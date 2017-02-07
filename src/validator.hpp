
#ifndef AutoNdn_VALIDATOR_HPP
#define AutoNdn_VALIDATOR_HPP

#include "common.hpp"
#include "security/certificate-store.hpp"

#include <ndn-cxx/security/validator-config.hpp>

namespace autondn {

class Validator : public ndn::ValidatorConfig
{
public:
  class Error : public ndn::ValidatorConfig::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : ndn::ValidatorConfig::Error(what)
    {
    }
  };

  explicit
  Validator(ndn::Face& face,
            const ndn::Name broadcastPrefix,
            const ndn::shared_ptr<ndn::CertificateCache>& cache,
            security::CertificateStore& certStore,
            const int stepLimit = 10)
    : ndn::ValidatorConfig(face, cache, ndn::ValidatorConfig::DEFAULT_GRACE_INTERVAL, stepLimit)
    , m_broadcastPrefix(broadcastPrefix)
    , m_certStore(certStore)
  {
    m_broadcastPrefix.append("KEYS");
  }

  virtual
  ~Validator()
  {
  }

  const ndn::Name&
  getBroadcastPrefix()
  {
    return m_broadcastPrefix;
  }

  void
  setBroadcastPrefix(const ndn::Name& broadcastPrefix)
  {
    m_broadcastPrefix = broadcastPrefix;
  }

protected:
  typedef std::vector<ndn::shared_ptr<ndn::ValidationRequest>> NextSteps;

  virtual void
  afterCheckPolicy(const NextSteps& nextSteps,
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

        Validator::onData(interest, *data, request);
      }
      else {
        // Prepend broadcast prefix to interest name
        ndn::Name broadcastName = m_broadcastPrefix;
        broadcastName.append(interest.getName());
        interest.setName(broadcastName);

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

  virtual ndn::shared_ptr<const ndn::Data>
  preCertificateValidation(const ndn::Data& data)
  {
    ndn::shared_ptr<ndn::Data> internalData = ndn::make_shared<ndn::Data>();
    internalData->wireDecode(data.getContent().blockFromValue());
    return internalData;
  }

private:
  ndn::Name m_broadcastPrefix;
  security::CertificateStore& m_certStore;
};

} // namespace autondn

#endif // AutoNdn_VALIDATOR_HPP
