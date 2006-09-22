#include "CondCore/DBCommon/interface/DBWriter.h"
#include "CondCore/DBCommon/interface/DBSession.h"
#include "CondCore/DBCommon/interface/Exception.h"
#include "CondCore/DBCommon/interface/ServiceLoader.h"
#include "CondCore/DBCommon/interface/ConnectMode.h"
#include "CondCore/DBCommon/interface/MessageLevel.h"
#include "CondCore/IOVService/interface/IOV.h"
#include "FWCore/Framework/interface/IOVSyncValue.h"
#include "CondCore/MetaDataService/interface/MetaData.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/MiscalibReaderFromXML.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/CaloMiscalibMap.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/MiscalibReaderFromXMLEcalBarrel.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/MiscalibReaderFromXMLEcalEndcap.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/CaloMiscalibMapEcal.h"

#include <string>
#include <iostream>

#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

using namespace std;

int main(){
// This part to read from xml
  CaloMiscalibMapEcal map_;
  map_.prefillMap();

  MiscalibReaderFromXMLEcalBarrel readerEB(map_);
  std::string xmlFile="miscalib_barrel_0.04.xml";
  if(! readerEB.parseXMLMiscalibFile(xmlFile) ){
     std::cout << "Parsing file: " << xmlFile << std::endl;
             } else {
     std::cout << "problems in parsing file: " << xmlFile << std::endl;
	     }


  MiscalibReaderFromXMLEcalEndcap readerEE(map_);
  xmlFile="miscalib_endcap_0.04.xml";
  if(! readerEE.parseXMLMiscalibFile(xmlFile) ){
     std::cout << "Parsing file: " << xmlFile << std::endl;
             } else {
     std::cout << "problems in parsing file: " << xmlFile << std::endl;
	     }
  
//
  cout << "Setting up DB Connection..." << flush;
  cond::ServiceLoader* loader = new cond::ServiceLoader;
  loader->loadMessageService(cond::Error);
  string contact("sqlite_file:ecalMiscalib.db");
  cond::DBSession* session = new cond::DBSession(contact);
  cond::MetaData* metadata_svc = new cond::MetaData(contact, *loader);
  cout << "Done." << endl;

try {
  cout << "Making Connections..." << flush;
  session->connect(cond::ReadWriteCreate);
  metadata_svc->connect();
  cout << "Done." << endl;
  
  cout << "Creating Writers..." << flush;
  cond::DBWriter icalWriter(*session, "EcalIntercalibConstants");
  cond::DBWriter iovWriter(*session, "IOV");
  cout << "Done." << endl;

  cout << "Starting Transaction." << endl;
  session->startUpdateTransaction();
  
 
  // get miscalic constants map
  
  EcalIntercalibConstants* ical = new EcalIntercalibConstants;

//  std::map<uint32_t,double> map = CaloMiscalibMap::getMap();

  std::map<uint32_t,float>::const_iterator it;

  for(it=map_.get().getMap().begin();it!=map_.get().getMap().end();it++){
      ical->setValue(it->first, it->second);
      cout << "first = " << it->first << " second= " << it->second << endl;
  }


  
  std::string ical_tok = icalWriter.markWrite<EcalIntercalibConstants>(ical);//pool::Ref takes the ownership of ped1

  // create IOV for EcalIntercalibConstants
  cond::IOV* ical_iov=new cond::IOV;
  ical_iov->iov.insert( std::make_pair(edm::IOVSyncValue::endOfTime().eventID().run(),ical_tok) );
  std::string ical_iov_Token = iovWriter.markWrite<cond::IOV>(ical_iov);
  std::cout << "Intercalib constants stored in DB with inf validity" << std::endl;
  
  cout << "Committing Session..." << endl;
  session->commit();
  cout << "Done." << endl;
  
  // Disconnect
  cout << "Disconnection Session..." << endl;
  session->disconnect();
  delete session;
  cout << "Done." << endl;

  cout << "Registering to MetaData Service..." << flush;
  metadata_svc->addMapping("EcalIntercalibConstants", ical_iov_Token);
  cout << "Done." << endl;

  cout << "Disconnecting MetaStata Service..." << flush;
  metadata_svc->disconnect();
  delete metadata_svc;
  cout << "Done." << endl;

  delete loader;

} catch(cond::Exception &e) {
  cout << e.what() << endl;
} catch(std::exception& e){
  cout << e.what() << endl;
} catch(...) {
  cout << "Unknown exception" << endl;
}
}
