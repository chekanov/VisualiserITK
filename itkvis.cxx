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
#include <ShowPixelSimpleService.h>

using namespace std;
using namespace xercesc;
using namespace InDet;


int main(int argc, char **argv)  
{
bool displayEndcaps = true;		//these booleans are used for debugging, set to false to hide certain parts
bool displayBarrelAndRings = true;
bool displaySimpleServices = true;
bool displayBeamPipe = true;
int argComplexity = 0;
vector <double> siAreaEndcaps;
vector <double> siAreaBarrel;
double totalSiAreaEndcaps = 0;
double totalSiAreaBarrel = 0;
double totalSiArea = 0;
// Look for a help argument
        for (int i = 1; i < argc; ++i) {
                const string arg = argv[i];
                if (arg == "--help" || arg == "-h") {
                        cout << argv[0] << ": itkvis visualiser" << endl;
                        cout << "Usage: " << argv[0] << "[<input geometry name from \"xml\" and [style 0, 1 oe 2]" << endl;
                        cout << "This runs a ITKvisualiser" << endl;
                        exit(0);
                }
        }

        
	
        string infile("-");
        string style("0");

        if (argc == 3) {
                infile = argv[1];
                style = argv[2];
        } else if (argc != 3) {
                cerr << "Usage: " << argv[0] << "[<input geometry name from \"xml\"] [style = 0, 1, 2]" << endl;
                exit(1);
        }

        argComplexity=atoi(style.c_str());


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
   geom->SetVisLevel(6); //makes it so that the geomanager will let you see up to 6 levels of daughters

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

   //--- make the inner container volume
   TGeoVolume *innerDetector = new TGeoVolumeAssembly("InnerTwoLayers");
   
   //--- make the outer container volume
   TGeoVolume *fullDetector = new TGeoVolumeAssembly("FullDetector");
   
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
   	
   // display pixel endcap 
   if(displayEndcaps){
   ShowPixelEndcap *pixelEndcap = new ShowPixelEndcap();
   siAreaEndcaps = pixelEndcap->process(reader,top, innerDetector, fullDetector, geom, argComplexity, infile);
   }
   
   //display pixel barrel
   if(displayBarrelAndRings){
   ShowPixelBarrel *pixelBarrel = new ShowPixelBarrel();
   siAreaBarrel = pixelBarrel->process(reader,top, innerDetector, fullDetector, geom, argComplexity,infile);
   }

    //display pixel simple services
    if(displaySimpleServices){ 
    ShowPixelSimpleService *pixelSimpleService = new ShowPixelSimpleService();
    pixelSimpleService->process(reader,top, innerDetector, fullDetector, geom, argComplexity);
    }


 	
   //add the inner assembly to the total volume if complexity is set to 3 otherwise add the whole detector
   if(argComplexity==3) top->AddNode(innerDetector,1,new TGeoTranslation(0,0,0));
   else top->AddNode(fullDetector,1,new TGeoTranslation(0,0,0));

   // beam pipe
   if(displayBeamPipe){
   TGeoVolume *BEAMPIPE = geom->MakeTube("BeamPipe", Be , 32, 32, 3500);
   BEAMPIPE->SetLineColor(43);
   BEAMPIPE->SetTransparency(50);
   top->AddNode(BEAMPIPE, 0, new TGeoTranslation(0,0,0));
   }

   //--- close the geometry
   geom->CloseGeometry();

   TEveManager::Create();
   TGeoNode* node = gGeoManager->GetTopNode();
   TEveGeoTopNode* en = new TEveGeoTopNode(gGeoManager, node);
   en->SetVisLevel(6);
   en->GetNode()->GetVolume()->SetVisibility(kFALSE);
   gEve->AddGlobalElement(en);
   TGLViewer *v = gEve->GetDefaultGLViewer();
   // v->GetClipSet()->SetClipType(1);
   v->ColorSet().Background().SetColor(kWhite);
   // v->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
   v->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0);
   gEve->Redraw3D(kTRUE);
   en->ExpandIntoListTreesRecursively();

   //siArea info 
   std::ofstream ofsSiArea;
   string out="out/"+infile+"_SiArea_"+style+".txt";
   ofsSiArea.open(out,ios::app);

   //siArea for Barrel 
   for(int iB = 0; iB<siAreaBarrel.size();iB++){
     cout<<"Silicon area in layer: "<<iB<<" of the Barrel and Rings assembly has: "<<siAreaBarrel[iB]<<"  mm^2 si"<<endl;
     ofsSiArea<<"Silicon area in layer: "<<iB<<" of the Barrel and Rings assembly has: "<<siAreaBarrel[iB]<<"  mm^2 si"<<endl;
     totalSiAreaBarrel += siAreaBarrel[iB];
   }

   cout<<"Area of Silicon in Barrel:                                    "<<totalSiAreaBarrel<<" mm^2"<<endl;
   ofsSiArea<<"Area of Silicon in Barrel:                                    "<<totalSiAreaBarrel<<" mm^2"<<endl;

   //si area for endcaps
   for(int iE = 0; iE<siAreaEndcaps.size();iE++){
     cout<<"Silicon area in layer: "<<iE<<" of the Endcaps assembly has:         "<<siAreaEndcaps[iE]<<"  mm^2 si"<<endl;
     ofsSiArea<<"Silicon area in layer: "<<iE<<" of the Endcaps assembly has:         "<<siAreaEndcaps[iE]<<"  mm^2 si"<<endl;
     totalSiAreaEndcaps += siAreaEndcaps[iE];
   }

   cout<<"Area of Silicon in Endcaps:                                   "<<totalSiAreaEndcaps<<" mm^2"<<endl;
   ofsSiArea<<"Area of Silicon in Endcaps:                                   "<<totalSiAreaEndcaps<<" mm^2"<<endl;
   totalSiArea = totalSiAreaEndcaps + totalSiAreaBarrel;
   cout<<"Area of Silicon in total:                                     "<<totalSiArea<<" mm^2"<<endl;
   ofsSiArea<<"Area of Silicon in total:                                     "<<totalSiArea<<" mm^2"<<endl;

   ofsSiArea.close();

   // look at TOP level
   TGeoVolume* itktop = gGeoManager->GetTopVolume();
   itktop->SetVisibility(kFALSE); // hide TGeoBBox shape in top volume

   out="out/"+infile+"_"+style+".root";
   cout << "Write file: " << out << endl;
   TFile* f = TFile::Open(out.c_str(),"recreate");
   itktop->Write("topvolume");
   delete f;


// this is fall-back solution, should work with most ROOT versions
  
   out="out/"+infile+"_"+style+".json";
   cout << "Write file: " << out << endl;
   TString json = TBufferJSON::ConvertToJSON(itktop, 3);
   std::ofstream ofs(out.c_str());
   ofs << json.Data();
   ofs.close();

   out="out/"+infile+"_"+style+".png";
   cout << "Write file: " << out << endl;
   gGeoManager->SaveAs(out.c_str());


   out="out/"+infile+"_"+style+".gdml";
   cout << "Write file: " << out << endl;
   gGeoManager->SaveAs(out.c_str());
   gGeoManager->Export(out.c_str());

   

  theApp.Run();
  return 0;
}
