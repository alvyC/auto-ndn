#include "conf-parameter.hpp"
#include <boost/test/unit_test.hpp>

namespace autondn {

namespace test {

using namespace std;

BOOST_AUTO_TEST_SUITE(TestConfParameter)

BOOST_AUTO_TEST_CASE(ConfParameterSettersAndGetters)
{

  ConfParameter cp1;

  const string NETWORK   = "autondn";
  const string CAR_MAKE  = "toyota";
  const string CAR_MODEL = "camri";
  const string CAR_ID    = "789F12345E";

  cp1.setNetwork(NETWORK);

  cp1.setCarMake(CAR_MAKE);

  cp1.setCarModel(CAR_MODEL);

  cp1.setCarId(CAR_ID);

  BOOST_CHECK_EQUAL(cp1.getNetwork(), "autondn");

  BOOST_CHECK_EQUAL(cp1.getCarMake(), "toyota");

  BOOST_CHECK_EQUAL(cp1.getCarModel(), "camri");

  BOOST_CHECK_EQUAL(cp1.getCarId(), "789F12345E");

  cp1.buildCarName();

  BOOST_CHECK_EQUAL(cp1.getCarName(), "/toyota/camri/789F12345E");
}

BOOST_AUTO_TEST_SUITE_END()

} //end of namespace test
} //end of namespace autondn
