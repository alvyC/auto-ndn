
#include <ndn-cxx/name.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <iostream>
#include <fstream>
#include "conf-file-processor.hpp"

//#include "auto-ndn.hpp"

namespace autondn {
using namespace std;
ConfFileProcessor::ConfFileProcessor(AutoNdn& autondn, std::string& confFileName)
  : m_confFileName(confFileName)
  , m_autondn(autondn)
{
}


bool
ConfFileProcessor::processConfFile() {

  bool ret = true;
  ifstream inputFile;
  inputFile.open(m_confFileName.c_str());
  if (!inputFile.is_open()) {
    string msg = "Failed to read configuration file: ";
    msg += m_confFileName;
    cerr << msg << std::endl;
    return false;
  }
  ret = load(inputFile);
  inputFile.close();
  return ret;
}

bool
ConfFileProcessor::load(std::istream& input) {
   ConfigSection pt;
   bool ret = true;
   try {
    boost::property_tree::read_info(input, pt);
  }
  catch (const boost::property_tree::info_parser_error& error) {
    stringstream msg;
    std::cerr << "Failed to parse configuration file " << std::endl;
    std::cerr << m_confFileName << std::endl;
    return false;
  }

  for (ConfigSection::const_iterator tn = pt.begin();
          tn != pt.end(); ++tn) {
            ret = processSection(tn->first, tn->second);
            if (ret == false) {
              break;
            }
  }
  return ret;
}

bool
ConfFileProcessor::processSection(const std::string& sectionName, const ConfigSection& section) {
  bool ret = true;
  if (sectionName == "general") {
    ret = processSectionGeneral(section);
  }
  else if (sectionName == "security") {
    ret = processSectionSecurity(section);
  }
  else {
    cerr << "Wrong configuration section: " << sectionName << endl;
  }
  return ret;
}

bool
ConfFileProcessor::processSectionGeneral(const ConfigSection& section) {
  try {
    string network = section.get<string>("network");
    string make = section.get<string>("make");
    string id = section.get<string>("id");

    ndn::Name networkName(network);
    if (!networkName.empty()) {
      m_autondn.getConfParameter().setNetwork(networkName);
    }
    else {
      cerr << "Network name cannot be empty" << endl;
    }

    ndn::Name makeName(make);
    if(!makeName.empty()) {
      m_autondn.getConfParameter().setCarMake(makeName);
    }
    else {
      cerr << "Make name cannot be empty" << endl;
    }

    ndn::Name idName(id);
    if (!idName.empty()) {
      // set the id name in confparameter
      m_autondn.getConfParameter().setCarId(idName);
    }
    else {
      cerr << "Car ID cannot be empty" << endl;
    }
  }
  catch(const std::exception& ex) {
    cerr << ex.what() << endl;
    return false;
  }
  return true;
}

bool
ConfFileProcessor::processSectionSecurity(const ConfigSection& section) {
  ConfigSection::const_iterator  it = section.begin();

  if (it == section.end() ||  it->first != "validator") {
    std::cerr << "Error: Expect validator section!" << std::endl;
    return false;
  }

  m_autondn.loadValidator(it->second, m_confFileName);

  return true;
}

} // end of namespace autondn
