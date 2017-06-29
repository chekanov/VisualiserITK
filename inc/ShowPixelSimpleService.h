#ifndef SHOWPIXELSIMPLESERVICE_H
#define SHOWPIXELSIMPLESERVICE_H

// STD library
 #include <iostream>
 #include <string>
 #include <vector>

#include <TROOT.h>
#include <TFile.h>
#include "TApplication.h"
#include "TObject.h"
#include "TMath.h"
#include "TCanvas.h"
#include <TGeoManager.h>
#include "TStopwatch.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"
#include "TGeoShapeAssembly.h"

#include <XMLGeoTemplates.h>
#include "XMLReaderSvc.h"
#include "XMLUtils.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <ShowPixelSimpleService.h>

using namespace std;
using namespace xercesc;
using namespace InDet;


class ShowPixelSimpleService {

  public:

    ShowPixelSimpleService(){};

    virtual ~ShowPixelSimpleService(){};

    double process(InDet::XMLReaderSvc& reader, TGeoVolume* top,TGeoVolume* innerDetector, TGeoVolume* outerDetector, TGeoManager *geom, int complexity);


};



#endif // SHOWPIXELESIMPLESERVICE_H

