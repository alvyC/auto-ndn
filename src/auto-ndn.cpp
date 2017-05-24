#define BOOST_LOG_DYN_LINK 1
#include "logging.hpp"
#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/security/v1/certificate.hpp>
#include <ndn-cxx/security/v1/public-key.hpp>
#include <ndn-cxx/security/transform/public-key.hpp>
#include <ndn-cxx/encoding/buffer.hpp>
#include <ndn-cxx/security/pib/identity.hpp>

#include <iostream>

#include "auto-ndn.hpp"

namespace autondn {

_LOG_INIT(autondn);

AutoNdn::AutoNdn(ndn::Face& face, ndn::util::Scheduler& scheduler)
  : m_face(face)
  , m_scheduler(scheduler)
  , m_control(scheduler, m_communication)
  , m_communication(*this, m_face, m_control, m_keyChain)
  , m_confParameter()
  , m_certificateCache(new ndn::CertificateCacheTtl(m_face.getIoService()))
  , m_validator(m_face, DEFAULT_BROADCAST_PREFIX, m_certificateCache, m_certStore)
  {
  }

  void
  AutoNdn::run() {
    //std::cout << "In AutoNdn::run() " << std::endl;
    initialize();
    m_control.run();

    try {
      m_communication.runProducer();
    }
    catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  void
  AutoNdn::initializeKey() {
    ndn::Name defaultIdentity = m_confParameter.getCarName();
    m_signingInfo = ndn::security::SigningInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID, defaultIdentity);
  }

  void
  AutoNdn::initialize() {
    m_confParameter.buildCarName();
    // TODO: initialize m_vehicleCurrentPnym and schedule generate and add Pseudonym to the pseudonym list

    _LOG_DEBUG("Vehicle name: " << m_confParameter.getCarName());
    initializeKey();
    setKeyInterestFilter();
  }

  void
  AutoNdn::setKeyInterestFilter() {
    ndn::Name keyPrefix = DEFAULT_BROADCAST_PREFIX;
    keyPrefix.append("KEYS");
    m_face.setInterestFilter(keyPrefix,
			     std::bind(&AutoNdn::onKeyInterest,
				       this, _1, _2),
			     std::bind(&AutoNdn::onKeyPrefixRegSuccess, this, _1),
		       std::bind(&AutoNdn::onRegistrationFailed, this, _1),
			     m_signingInfo);
  }

  void
  AutoNdn::onKeyInterest(const ndn::Name& name, const ndn::Interest& interest) {
    const ndn::Name& interestName = interest.getName();
    ndn::Name certName = interestName.getSubName(name.size());

    _LOG_TRACE("Got interest for certificate: " << certName);

    ndn::shared_ptr<const ndn::IdentityCertificate> cert = getCertificate(certName);

    if (!static_cast<bool>(cert))
      return; // cert is not found

    ndn::shared_ptr<ndn::Data> data = ndn::make_shared<ndn::Data>();
    data->setName(interestName);
    data->setContent(cert->wireEncode());
    m_keyChain.sign(*data, m_signingInfo);

    m_face.put(*data);
  }

  void
  AutoNdn::onKeyPrefixRegSuccess(const ndn::Name& name) {
  }

  void
  AutoNdn::onRegistrationFailed(const ndn::Name& name) {
  }

  ndn::Name
  AutoNdn::getNewPseudonym() {
    ndn::Name vehicleNewPnym("/pseudonym1");
    m_makeCurrentPnym = m_confParameter.getCarMake();
    vehicleNewPnym.append(m_makeCurrentPnym);

    return vehicleNewPnym;
  }

  void
  AutoNdn::generateAndAddPseudonym() {
    /* (1) generate vehicle pseudonym: <vehicle-pnym>/ <make-pnym>
       (2) Create key for the pseudonym
    */
    ndn::Name vehicleNewPnym = getNewPseudonym();

    // generate key for the new pseudonym
    // ndn::Name keyName = m_keyChain.generateRsaKeyPairAsDefault(vehicleNewPnym, true);
    ndn::security::SigningInfo signingInfo(ndn::security::SigningInfo::SIGNER_TYPE_ID, vehicleNewPnym);
    //std::shared_ptr<ndn::PublicKey> pubKey = m_keyChain.getPublicKey(keyName);

    /*(3) Initiate the certificate retrieval process by asking for proxy's key, Interest: /autondn/CIP/request-key
          autondn-cip: onKeyRequestInitInterest()*/
    ndn::Interest interest("/autondn/CIP/request-key");
    m_face.expressInterest(interest,
                           std::bind(&AutoNdn::requestCertForPnym, this, _1, _2, vehicleNewPnym), // step (4) and (5) here
                           std::bind([] {}));
    /*(4) After getting proxy key, create an encrypted interest
           Interest: /autondn/CIP/<cip-id>/E-CIP{manufacturer, E-Man{vid, K-VCurr, K-VNew}}
           autondn-cip: onCertInterest()

      (5) Send the Interest to manufacturer via proxy for cert

      step (4) and (5) are done in requestCertForPnym().
    */
  }

  void
  AutoNdn::requestCertForPnym(const ndn::Interest& i, const ndn::Data& d, const ndn::Name& vehicleNewPnym) {
    ndn::Name interestName("/autondn/CIP");

    // get the name (id) of the proxy from interest and append it to the interest name; interest name: /autondn/CIP/<proxy-id>
    interestName.append(ndn::Name(i.getName().get(-1)));

    // make certificate object from the certificate data packet
    ndn::security::v1::Certificate cert(d);
    ndn::security::v1::PublicKey proxyKeyInfo = cert.getPublicKeyInfo();

    ndn::security::transform::PublicKey proxyKey;
    proxyKey.loadPkcs8(proxyKeyInfo.get().buf(), proxyKeyInfo.get().size());

    // encrypt make's name using proxy's key
    std::string makeName = m_confParameter.getCarMake().toUri();
    std::vector<uint8_t> myVector(makeName.begin(), makeName.end());
    uint8_t *p = &myVector[0];
    ndn::ConstBufferPtr cipherMakeName = proxyKey.encrypt(p, myVector.size());

    // append encrypted make name to the interest; interest name: /autondn/CIP/<proxy-id>/E-proxy{make-name}
    interestName.append(cipherMakeName->buf(), cipherMakeName->size());

    // get manufacturer's key and load it
    ndn::Name manKeyName = m_keyChain.getDefaultKeyNameForIdentity(m_makeCurrentPnym);
    std::shared_ptr<ndn::PublicKey> manPubKeyInfo = m_keyChain.getPublicKey(manKeyName);
    ndn::security::transform::PublicKey manKey;
    manKey.loadPkcs8(manPubKeyInfo->get().buf(), manPubKeyInfo->get().size());

    // Get vehicle's current key
    ndn::Name vehicleCurrentKeyName = m_keyChain.getDefaultKeyNameForIdentity(m_vehicleCurrentPnym);
    std::shared_ptr<ndn::PublicKey> vehicleCurrentKeyInfo = m_keyChain.getPublicKey(vehicleCurrentKeyName);

    // Get vehicle's new key
    ndn::Name vehicleNewKeyName = m_keyChain.getDefaultKeyNameForIdentity(vehicleNewPnym);
    std::shared_ptr<ndn::PublicKey> vehicleNewKeyInfo = m_keyChain.getPublicKey(vehicleNewKeyName);

    // Encrypt vehicle's current key using manufacturer's key
    ndn::ConstBufferPtr cipherVehicleCurrentKey = manKey.encrypt(vehicleCurrentKeyInfo->get().buf(), vehicleCurrentKeyInfo->get().size());
    interestName.append(cipherVehicleCurrentKey->buf(), cipherVehicleCurrentKey->size());

    // Encrypt vehicle's new key using manufacturer's key
    ndn::ConstBufferPtr cipherVehicleNewKey = manKey.encrypt(vehicleNewKeyInfo->get().buf(), vehicleNewKeyInfo->get().size());
    interestName.append(cipherVehicleNewKey->buf(), cipherVehicleNewKey->size());

    //Interest: /autondn/CIP/<cip-id>/E-CIP{manufacturer}/ E-Man{vid}/E-man{K-VCurr}, E-man{K-VNew}
    ndn::Interest interest(interestName);
    m_face.expressInterest(interest,
                           std::bind(&AutoNdn::installVehicleCert, this, _1, _2, vehicleCurrentKeyName), // got certificate, now install it
                           std::bind([]{})); // timeout
  }

  void
  AutoNdn::installVehicleCert(const ndn::Interest& i, const ndn::Data& d,
                              const ndn::Name& vehicleCurrentKeyName) {
    /* Data received from proxy contains certificate signed by manufacturer.
       Data is encrypted with vehicle's key (vehicleCurrentKeyName), so need to decrypt it first
    */

  }
} // end of namespace autondn
