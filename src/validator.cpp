#define BOOST_LOG_DYN_LINK 1

#include "validator.hpp"
#include "logging.hpp"

namespace autondn {

_LOG_INIT(validator);

  
  Validator::Validator(ndn::Face& face,
            const ndn::Name broadcastPrefix,
            const ndn::shared_ptr<ndn::CertificateCache>& cache,
            security::CertificateStore& certStore,
            const int stepLimit)
    : ndn::ValidatorConfig(face, cache, ndn::ValidatorConfig::DEFAULT_GRACE_INTERVAL, stepLimit)
    , m_broadcastPrefix(broadcastPrefix)
    , m_certStore(certStore)
    , m_stepLimit(stepLimit) 
  {
    m_broadcastPrefix.append("KEYS");
  }

  Validator::~Validator()
  {
  }


  void
  Validator::afterCheckPolicy(const NextSteps& nextSteps,
                              const OnFailure& onFailure)
  {
    if (m_face == nullptr) {
      onFailure("Require more information to validate the packet!");
      return;
    }

    for (const shared_ptr<ndn::ValidationRequest>& request : nextSteps) {

      ndn::Interest& interest = request->m_interest;

      // Look for certificate in permanent storage
      shared_ptr<const ndn::IdentityCertificate> cert = m_certStore.find(interest.getName());

      if (cert != nullptr) {
        // If the certificate is found, no reason to express interest
        shared_ptr<ndn::Data> data = make_shared<ndn::Data>(interest.getName());
        data->setContent(cert->wireEncode());
	//_LOG_DEBUG("Certificate found in cache: ");
        onData(interest, *data, request);
      }
      else {
        // Prepend broadcast prefix to interest name
        ndn::Name broadcastName = m_broadcastPrefix;
        broadcastName.append(interest.getName());
        interest.setName(broadcastName);
  	_LOG_DEBUG("Express interest for certificate: "<< interest.getName());
        // Attempt to fetch the certificate
        m_face->expressInterest(interest,
                                bind(&Validator::onData, this, _1, _2, request),
                                bind(&Validator::onTimeout,
                                     this, _1, request->m_nRetries,
                                     onFailure,
                                     request));
      }
    }
  }

  ndn::shared_ptr<const ndn::Data>
  Validator::preCertificateValidation(const ndn::Data& data)
  {
    ndn::shared_ptr<ndn::Data> internalData = ndn::make_shared<ndn::Data>();
    internalData->wireDecode(data.getContent().blockFromValue());
    return internalData;
  }

  void
  Validator::onData(const ndn::Interest& interest,
                  const ndn::Data& data,
                  const shared_ptr<ndn::ValidationRequest>& nextStep)
 {
  _LOG_DEBUG("Got certificate " << interest.getName());
  shared_ptr<const ndn::Data> certificateData = preCertificateValidation(data);

  if (!static_cast<bool>(certificateData))
    return nextStep->m_onDataValidationFailed(data.shared_from_this(),
                                              "Cannot decode cert: " + data.getName().toUri());

  validate(*certificateData,
           nextStep->m_onDataValidated, nextStep->m_onDataValidationFailed,
           nextStep->m_nSteps);
 }


} // namespace autondn
