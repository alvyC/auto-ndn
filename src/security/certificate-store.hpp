#ifndef AUTONDN_CERTIFICATE_STORE_HPP
#define AUTONDN_CERTIFICATE_STORE_HPP

#include <ndn-cxx/security/v1/identity-certificate.hpp>

#include <map>

namespace autondn {
namespace security {

class CertificateStore
{
public:
 void
 insert(std::shared_ptr<ndn::IdentityCertificate> certificate) {
  if (certificate != nullptr) {
    m_certificates[certificate->getName().getPrefix(-1)] = certificate;
  }
 }

std::shared_ptr<const ndn::IdentityCertificate>
find(const ndn::Name& certificateNameWithoutVersion) const {
  CertMap::const_iterator it = m_certificates.find(certificateNameWithoutVersion);

  if (it != m_certificates.end()) {
    return it->second;
  }

  return nullptr;
}

void
clear() {
  m_certificates.clear();
}

private:
 typedef std::map<ndn::Name, std::shared_ptr<ndn::IdentityCertificate>> CertMap;
 CertMap m_certificates;
};
} // end of namespace security
} // end of namespace autondn

#endif // AUTONDN_CERTIFICATE_STORE_HPP
