#include "pseudonym-pool.hpp"

namespace autondn {

PseudonymPool::PseudonymPool(ndn::util::Scheduler& scheduler)
	: m_scheduler(scheduler)
	{}

void
PseudonymPool::addPseudonym(Pseudonym pnym) {
	// add a pseudonym and schedule removal of the pseudonym according to timepoint
	m_pnymPool.push_back(pnym);
	m_scheduler.scheduleEvent(pnym.getExpirationTimepoint(), std::bind(&PseudonymPool::removePseudonym, this, pnym));
}

void
PseudonymPool::removePseudonym(Pseudonym pnym) {
	std::vector<Pseudonym>::iterator it = std::find_if(m_pnymPool.begin(), m_pnymPool.end(),
													   std::bind(&PseudonymPool::isPseudonymEqual, this, _1, pnym));

	if (it != m_pnymPool.end()) {
		m_pnymPool.erase(it);
	}
}

bool
PseudonymPool::isPseudonymEqual(Pseudonym pnym1, Pseudonym pnym2) {
	return (pnym1 == pnym2);
}

}