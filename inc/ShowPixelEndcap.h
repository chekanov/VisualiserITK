///////////////////////////////////////////////////////////////////

#ifndef SHOWPIXELENDCAP_H
#define SHOWPIXELENDCAP_H

// STD library
#include <iostream>
#include <string>
#include <vector>
#include <TROOT.h>
#include <TFile.h>
#include "TApplication.h"
#include "TObject.h"
#include "TMath.h"
#include "TThread.h"
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

#include <ShowPixelEndcap.h>

using namespace std;
using namespace xercesc;
using namespace InDet;


class ShowPixelEndcap {

  public:

    ShowPixelEndcap(){};

    virtual ~ShowPixelEndcap(){};

    bool process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoManager *geom);

};



#endif // SHOWPIXELENDCAP_H

