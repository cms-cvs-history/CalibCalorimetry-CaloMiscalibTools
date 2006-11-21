#include "PluginManager/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/SourceFactory.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/EcalRecHitRecalib.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/CaloMiscalibTools.h"
#include "CalibCalorimetry/CaloMiscalibTools/interface/HcalRecHitRecalib.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(EcalRecHitRecalib);
DEFINE_ANOTHER_FWK_MODULE(HcalRecHitRecalib);
DEFINE_ANOTHER_FWK_EVENTSETUP_SOURCE(CaloMiscalibTools);