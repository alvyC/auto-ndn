#include "pseudonym-pool.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>

#include <boost/test/unit_test.hpp>

namespace autondn {

namespace tests {

BOOST_AUTO_TEST_SUITE(TestPseudonymPool)

BOOST_AUTO_TEST_CASE(PseudonymIsEqual) {
	Pseudonym pnym1(ndn::Name("1234"), ndn::Name("toyota"), ndn::time::seconds(3600));
	Pseudonym pnym2(ndn::Name("5678"), ndn::Name("honda"), ndn::time::seconds(3600));

	// pnym1 and pnym2 are not equal
	BOOST_CHECK_EQUAL(pnym1 == pnym2, false);

	Pseudonym pnym3(ndn::Name("5678"), ndn::Name("honda"), ndn::time::seconds(3600));

	BOOST_CHECK_EQUAL(pnym2 == pnym3, true);
}

BOOST_AUTO_TEST_CASE(RemovalOnExpiration) {
	Pseudonym pnym1(ndn::Name("1234"), ndn::Name("toyota"), ndn::time::seconds(3600));

	ndn::util::DummyClientFace face;
	ndn::util::scheduler::Scheduler scheduler(face.getIoService());
	PseudonymPool pnymPool(scheduler);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
