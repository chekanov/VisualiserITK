///////////////////////////////////////////////////////////////////

#ifndef MATERIAL_H
#define MATERIAL_H

// STD library
#include <iostream>
#include <string>
#include <vector>


#include <TROOT.h>
#include <TMath.h> 
#include <TGeoManager.h>
#include "TStopwatch.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"
#include "TGeoShapeAssembly.h"

using namespace std;


class Material {

  public:

  TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
  TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);


};



#endif // SHOWPIXELBARREL_H

