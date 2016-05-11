#ifndef CONF_FILE_PROCESSOR_HPP
#define CONF_FILE_PROCESSOR_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/cstdint.hpp>

#include "auto-ndn.hpp"

namespace autondn {

class ConfFileProcessor {
public:
ConfFileProcessor(AutoNdn& autondn, std::string& confFileName);
 
 bool
 processConfFile();

private:
 typedef boost::property_tree::ptree ConfigSection;

 bool
 load(std::istream& input);

 bool
 processSection(const std::string& sectionName, const ConfigSection& section);

 bool
 processSectionGeneral(const ConfigSection& section);

 bool
 processSectionSecurity(const ConfigSection& section);

private:
 std::string m_confFileName;
 AutoNdn& m_autondn;
};

} // end of namespace autondn

#endif // CONF_FILE_PROCESSOR_HPP