#include "pseudonym.hpp"

namespace autondn {

	Pseudonym::Pseudonym(ndn::Name vehiclePart, ndn::Name manuPart, ndn::time::seconds expirationTimepoint)
	  : m_vehiclePart(vehiclePart)
	  , m_manuPart(manuPart)
	  , m_pnym(m_manuPart.append(m_vehiclePart))
	  , m_expirationTimepoint(expirationTimepoint)
	  {
	  }

	ndn::Name
	Pseudonym::getPNym() const {
		return m_pnym;
	}

	ndn::time::seconds
	Pseudonym::getExpirationTimepoint() const {
		return m_expirationTimepoint;
	}

	bool
	operator==(const Pseudonym& lhs, const Pseudonym& rhs) {
		return (lhs.m_vehiclePart == rhs.m_vehiclePart) && (lhs.m_manuPart == rhs.m_manuPart);
	}
}