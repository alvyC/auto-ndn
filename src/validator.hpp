#define BOOST_LOG_DYN_LINK 1

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
            const int stepLimit = 10);

  virtual
  ~Validator();

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
                   const OnFailure& onFailure);

  virtual ndn::shared_ptr<const ndn::Data>
  preCertificateValidation(const ndn::Data& data);

  void
  onData(const ndn::Interest& interest,
                  const ndn::Data& data,
                  const shared_ptr<ndn::ValidationRequest>& nextStep);

private:
  ndn::Name m_broadcastPrefix;
  security::CertificateStore& m_certStore;
  int m_stepLimit;
};

} // namespace autondn

#endif // AutoNdn_VALIDATOR_HPP
