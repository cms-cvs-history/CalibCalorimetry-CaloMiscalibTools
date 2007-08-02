// -*- C++ -*-
//
// Package:    WriteEcalMiscalibConstants
// Class:      WriteEcalMiscalibConstants
// 
/**\class WriteEcalMiscalibConstants WriteEcalMiscalibConstants.cc CalibCalorimetry/WriteEcalMiscalibConstants/src/WriteEcalMiscalibConstants.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Stephanie BEAUCERON
//         Created:  Tue May 15 16:23:21 CEST 2007
// $Id: WriteEcalMiscalibConstants.cc,v 1.1 2007/07/14 21:25:59 malgeri Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

// DB includes
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

// user include files
#include "CondFormats/HcalObjects/interface/HcalGains.h"
#include "CondFormats/DataRecord/interface/HcalGainsRcd.h"
//For Checks
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//this one
#include "CalibCalorimetry/CaloMiscalibTools/interface/WriteHcalGains.h"

//
// static data member definitions
//

//
// constructors and destructor
//
WriteHcalGains::WriteHcalGains(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed
  newTagRequest_ = iConfig.getParameter< std::string > ("NewTagRequest");


}


WriteHcalGains::~WriteHcalGains()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
WriteHcalGains::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
}


// ------------ method called once each job just before starting event loop  ------------
void 
WriteHcalGains::beginJob(const edm::EventSetup& iSetup)
{
   using namespace edm;
  // Intercalib constants
   edm::ESHandle<HcalGains> pIcal;
   iSetup.get<HcalGainsRcd>().get(pIcal);

   //   const HcalGains* Mcal = pIcal.product();

   HcalGains* Mcal = new HcalGains(*(pIcal.product()));
   
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  if( poolDbService.isAvailable() ){
    if ( poolDbService->isNewTagRequest(newTagRequest_) ){
      std::cout<<" Creating a  new one "<<std::endl;
      poolDbService->createNewIOV<const HcalGains>( Mcal, poolDbService->endOfTime(),newTagRequest_);
      std::cout<<"Done" << std::endl;
    }else{
      std::cout<<"Old One "<<std::endl;
       poolDbService->appendSinceTime<const HcalGains>( Mcal, poolDbService->currentTime(),newTagRequest_);
    }
  }  
}

// ------------ method called once each job just after ending the event loop  ------------
void 

WriteHcalGains::endJob() {
  std::cout << "Here is the end" << std::endl; 
}

