#if !defined(__CLING__) || defined(__ROOTCLING__)

//#include "MCHGeometryTest/Helpers.h"
#include "DetectorsBase/GeometryManager.h"
#include "DetectorsBase/MaterialManager.h"

#include "MCHGeometryMisAligner/MisAligner.h"
#include "MCHGeometryTransformer/Transformations.h"

#include "MCHGeometryTest/Helpers.h"
#include "MCHGeometryCreator/Geometry.h"

#include "Math/GenVector/Cartesian3D.h"
#include "TGLRnrCtx.h"
#include "TGLViewer.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TH2F.h"
#include "TPRegexp.h"
#include "TVirtualPad.h"

#include <iostream>

#endif

//void misAlign()
void misAlign(Double_t xcartmisaligm = 0.0, Double_t xcartmisaligw = 0.004,
                      Double_t ycartmisaligm = 0.0, Double_t ycartmisaligw = 0.003,
                      Double_t angmisaligm = 0.0, Double_t angmisaligw = 0.0023,
//                      TString nameCDB = "ResMisAlignCDB",
                      const std::string& fileName = "MCHMisAlignment.root")
{

    // create a regular geometry
    createStandaloneGeometry();
    if (!gGeoManager) {
        std::cerr << "gGeoManager == nullptr, must create a geometry first\n";
        return;
    }
    // If not closed, we need to close it
    if (!gGeoManager->IsClosed()) {
        gGeoManager->CloseGeometry();
    }
    // Then add the alignable volumes
    addAlignableVolumes();
    
    // The misaligner
    o2::mch::geo::MisAligner aGMA(xcartmisaligm, xcartmisaligw, ycartmisaligm, ycartmisaligw);
    //aGMA.SetCartMisAlig(xcartmisaligm, xcartmisaligw, ycartmisaligm, ycartmisaligw);
    aGMA.SetAngMisAlig(angmisaligm, angmisaligw);
    
    // To generate module mislaignment (not mandatory)
    aGMA.SetModuleCartMisAlig(0.0, 0.003, 0.0, 0.003, 0.0, 0.003);
    aGMA.SetModuleAngMisAlig(0.0, 0.0006, 0.0, 0.001, 0.0, 0.0005);
    
    aGMA.MisAlign();
    
    /*
     // A faire plus tard, pris de la macro dans AliRoot
    // Generate misaligned data in local cdb
    const TClonesArray* array = newTransform->GetMisAlignmentData();
    
    // 100 mum residual resolution for chamber misalignments?
    misAligner.SetAlignmentResolution(array,-1,0.01,0.01,xcartmisaligw,ycartmisaligw);
     */
    
    // la suite est prise de la fonction MisAlign
    const std::string& ccdbHost = "http://localhost:8080";
    long tmin = 0;
    long tmax = -1;
    const std::string& objectPath = "";
    
    if (!ccdbHost.empty()) {
        std::string path = objectPath.empty() ? o2::base::NameConf::getAlignmentPath(detMCH) : objectPath;
        LOGP(info, "Storing alignment object on {}/{}", ccdbHost, path);
        o2::ccdb::CcdbApi api;
        map<string, string> metadata; // can be empty
        api.init(ccdbHost.c_str());   // or http://localhost:8080 for a local installation
        // store abitrary user object in strongly typed manner
        api.storeAsTFileAny(&params, path, metadata, tmin, tmax);
    }
    
    if (!fileName.empty()) {
        LOGP(info, "Storing ITS alignment in local file {}", fileName);
        TFile algFile(fileName.c_str(), "recreate");
        algFile.WriteObjectAny(&params, "std::vector<o2::detectors::AlignParam>", "alignment");
        algFile.Close();
    }
    
    //o2::mch::test::misAlignGeometry();
}
