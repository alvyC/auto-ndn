#ifndef PSEUDONYM_POOL_HPP
#define PSEUDONYM_POOL_HPP

#include "pseudonym.hpp"

#include <ndn-cxx/util/scheduler.hpp>

#include <algorithm>

namespace autondn {

class PseudonymPool {
public:
	PseudonymPool(ndn::util::Scheduler&);

	void
	addPseudonym(Pseudonym pnym);

	void
	removePseudonym(Pseudonym pnym);

private:
	bool
	isPseudonymEqual(Pseudonym pnym1, Pseudonym pnym2);

private:
	std::vector<Pseudonym> m_pnymPool;
	ndn::util::Scheduler& m_scheduler;
};
}

#endif