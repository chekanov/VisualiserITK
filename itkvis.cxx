/***************************************************************************
 *            main.cxx
 *  Fri Sep 21 15:05:23 2007
 *  Copyright  2007  chekanov
 ****************************************************************************/



#include <iostream>
#include <fstream>
#include <stdlib.h>
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
#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TGeoBBox.h"
#include "TRotMatrix.h"
#include "TEveManager.h"
#include "TEveGeoNode.h"
#include "TShape.h" 
#include "TGLViewer.h"
#include "TBufferJSON.h"

#include "XMLReaderSvc.h"
#include "XMLUtils.h"

#include <ShowPixelBarrel.h>
#include <ShowPixelEndcap.h>


using namespace std;
using namespace xercesc;
using namespace InDet;


int main(int argc, char **argv)
{


// Look for a help argument
        for (int i = 1; i < argc; ++i) {
                const string arg = argv[i];
                if (arg == "--help" || arg == "-h") {
                        cout << argv[0] << ": itkvis visualiser" << endl;
                        cout << "Usage: " << argv[0] << "[<input geometry name from \"xml\"]" << endl;
                        cout << "This runs a ITKvisualiser" << endl;
                        exit(0);
                }
        }


        string infile("-");
        if (argc == 2) {
                infile = argv[1];
        } else if (argc != 3) {
                cerr << "Usage: " << argv[0] << "[<input geometry name from \"xml\"]" << endl;
                exit(1);
        }


#ifdef __CINT__
   gSystem->Load("libGeom");
#endif

  //gSystem->Load("libGeom");
  //gROOT->SetBatch();
  TApplication theApp("App", &argc, argv);


  MemInfo_t memInfo;
  gSystem->GetMemInfo(&memInfo);
  cout << " Mem Total = " << memInfo.fMemTotal << endl; 
  cout << " Mem Used = " << memInfo.fMemUsed << endl; 
  cout << " Mem Swapped = " << memInfo.fSwapUsed << endl; 
  int mStart=(int)memInfo.fMemUsed;


//--- Definition of a simple geometry
   gSystem->Load("libGeom");
   TGeoManager *geom = new TGeoManager("World of ITK", "Geometry of ITK");
   //--- define some materials
   TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
//   //--- define some media
   TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
   TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);

   //--- make the top container volume
   TGeoVolume *top = geom->MakeBox("TOP", Vacuum, 1000., 1000., 2000.);
   top->SetTransparency(70);
   geom->SetTopVolume(top);


   TGeoMaterial *mat;
   TGeoMixture *mix;
   //---> create some materials
   mat = new TGeoMaterial("Vacuum",0,0,0);
   mat->SetUniqueID(0);
   mat = new TGeoMaterial("Be", 9.01,4,1.848);
   mat->SetUniqueID(1);
   mat = new TGeoMaterial("Al", 26.98,13,2.7);
   mat->SetUniqueID(2);
   mat = new TGeoMaterial("Fe", 55.85,26,7.87);
   mat->SetUniqueID(3);
   mat = new TGeoMaterial("Cu", 63.55,29,8.96);
   mat->SetUniqueID(4);
   mat = new TGeoMaterial("C",12.01,6,2.265);
   mat->SetUniqueID(5);
   mat = new TGeoMaterial("Pb",207.19,82,11.35);
   mat->SetUniqueID(6);
   mat = new TGeoMaterial("Si",28.09,14,2.33);
   mat->SetUniqueID(7);
   mix = new TGeoMixture("scint",2,   1.03200    );
   mix->DefineElement(0,1.008,1,0.7749078E-01);
   mix->DefineElement(1,12,6,0.9225092);
   mix->SetUniqueID(8);
   mat = new TGeoMaterial("Li",6.94,3,0.534);
   mat->SetUniqueID(9);
   mat = new TGeoMaterial("N",14.01,7,0.808);
   mat->SetUniqueID(10);
   mat = new TGeoMaterial("Ne",20.18,10,1.207);
   mat->SetUniqueID(11);
   mat = new TGeoMaterial("Ts",183.85,74,19.3);
   mat->SetUniqueID(12);

//---> create mediums
TGeoMedium *Be = new TGeoMedium("Be",1,1,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *Fe = new TGeoMedium("Fe",3,3,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *Cu = new TGeoMedium("Cu",4,4,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *C = new TGeoMedium("C",5,5,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *Pb = new TGeoMedium("Pb",6,6,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *Si = new TGeoMedium("Si",7,7,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *Li = new TGeoMedium("Li",9,9,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *N = new TGeoMedium("N",10,10,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *Ne = new TGeoMedium("Ne",11,11,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
TGeoMedium *Ts = new TGeoMedium("Ts",12,12,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);

    // define geometry here
    XMLReaderSvc reader(infile);
    reader.initialize();

   /* ------------------- show each parts of the detector -------------------*/
   // display pixel barrel
   ShowPixelBarrel *pixelBarrel = new ShowPixelBarrel();
   pixelBarrel->process(reader,top, geom);
 
   // display pixel endcap 
   ShowPixelEndcap *pixelEndcap = new ShowPixelEndcap();
   pixelEndcap->process(reader,top, geom);
 

   // beam pipe
   TGeoVolume *BEAMPIPE = geom->MakeTube("BeamPipe", Be , 32, 32, 3500);
   BEAMPIPE->SetLineColor(43);
   BEAMPIPE->SetTransparency(50);
   top->AddNode(BEAMPIPE, 0, new TGeoTranslation(0,0,0));


   //--- close the geometry
   geom->CloseGeometry();

   TEveManager::Create();
   TGeoNode* node = gGeoManager->GetTopNode();
   TEveGeoTopNode* en = new TEveGeoTopNode(gGeoManager, node);
   en->SetVisLevel(4);
   en->GetNode()->GetVolume()->SetVisibility(kFALSE);
   gEve->AddGlobalElement(en);
   TGLViewer *v = gEve->GetDefaultGLViewer();
   // v->GetClipSet()->SetClipType(1);
   v->ColorSet().Background().SetColor(kWhite);
   // v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
   v->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0);
   gEve->Redraw3D(kTRUE);
   en->ExpandIntoListTreesRecursively();



   // look at TOP level
   TGeoVolume* itktop = gGeoManager->GetTopVolume();
   itktop->SetVisibility(kFALSE); // hide TGeoBBox shape in top volume

   string out="out/"+infile+".root";
   cout << "Write file: " << out << endl;
   TFile* f = TFile::Open(out.c_str(),"recreate");
   itktop->Write("topvolume");
   delete f;


// this is fall-back solution, should work with most ROOT versions
  
   out="out/"+infile+".json";
   cout << "Write file: " << out << endl;
   TString json = TBufferJSON::ConvertToJSON(itktop, 3);
   std::ofstream ofs(out.c_str());
   ofs << json.Data();
   ofs.close();

   out="out/"+infile+".png";
   cout << "Write file: " << out << endl;
   gGeoManager->SaveAs(out.c_str());


   out="out/"+infile+".gdml";
   cout << "Write file: " << out << endl;
   // gGeoManager->SaveAs(out.c_str());
   gGeoManager->Export(out.c_str());


  theApp.Run();
  return 0;
}
