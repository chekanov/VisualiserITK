#include "ShowPixelEndcap.h"

// implementation of the endcap drawing
bool ShowPixelEndcap::process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoManager* geom)
{


   // define some constants
   const float degree=57.2958;
   const float PI=TMath::Pi();
   const float PI2=2*TMath::Pi();


//--- define some materials
   TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
   TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
   TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);
   TGeoMedium *Si = new TGeoMedium("Si",7,7,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);


   return true;
} 



