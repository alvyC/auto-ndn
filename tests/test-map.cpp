#include "map/map.hpp"
#include <boost/test/unit_test.hpp>

namespace autondn {

namespace test {

BOOST_AUTO_TEST_SUITE(TestMap)

BOOST_AUTO_TEST_CASE(MapRemoveLinkTest)
{
  Map map1;

  std::string start = "1,1";
  std::string end = "4,4";

  map1.calculatePath(start, end);

  BOOST_CHECK_EQUAL(map1.getPath().size(), 7);

  //Remove the following link
  std::string n1 = "3,3";
  std::string n2 = "3,4";

  map1.removeLink(n1, n2);

  //Path from 3,3 to 4,4
  map1.calculatePath(n1, end);

  //Should be three now since we seek the path from 3,3 to 4,4
  BOOST_CHECK_EQUAL(map1.getPath().size(), 3);
}

BOOST_AUTO_TEST_SUITE_END()

} //namespace test
} //namespace autondn
