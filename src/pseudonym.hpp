#ifndef PSEUDONYM_HPP
#define PSEUDONYM_HPP

#include <ndn-cxx/name.hpp>
#include <ndn-cxx/util/time.hpp>

namespace autondn {

class Pseudonym {
public:
	Pseudonym(ndn::Name m_vehiclePart, ndn::Name m_manuPart, ndn::time::seconds m_expirationTimepoint);

	ndn::Name
	getPNym() const;

	ndn::time::seconds
	getExpirationTimepoint() const;
private:
	ndn::Name m_vehiclePart;
	ndn::Name m_manuPart;
	ndn::Name m_pnym;
	ndn::time::seconds m_expirationTimepoint;

	friend bool
	operator==(const Pseudonym& lhs, const Pseudonym& rhs);
};
}

#endif 