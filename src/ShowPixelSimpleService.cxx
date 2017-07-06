/*Implementation of Pixel Simple Services and endcap supports(simple service similar) for ITKVisualiser framework
*
* @author A.Fischer (LUC)
*/

#include "ShowPixelSimpleService.h"
#include <cmath>

// implementation of the SimpleServiceTubes
 double ShowPixelSimpleService::process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoVolume* innerDetector, TGeoVolume* fullDetector, TGeoManager* geom, int complexity)
 {


    // define some constants
       const double degree=57.2958;
       const double PI=TMath::Pi();
       const double PI2=2*TMath::Pi();
       
       double siArea = 0; //simple services should not add to si area

    //--- define some materials
	TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
        TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
        TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
        TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);
        TGeoMedium *Si = new TGeoMedium("Si",7,7,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);
	
	//create assemblies to hold volumes
        string innerPixelSimpleServiceName = "InnerPixelSimpleServices";
        TGeoVolume *innerSimpleServiceAssembly = new TGeoVolumeAssembly(innerPixelSimpleServiceName.c_str());
	string PixelSimpleServiceName = "PixelSimpleServices";
        TGeoVolume *SimpleServiceAssembly = new TGeoVolumeAssembly(PixelSimpleServiceName.c_str());


	std::vector < SimpleServiceTubeTmp *> simpleServiceTmps = reader.getPixelSimpleServiceTubeTemplates();
	
	//loop over all simple services
	for(int i=0;i<simpleServiceTmps.size();i++){
		SimpleServiceTubeTmp *simpleServiceTube =simpleServiceTmps[i];
		
		double dZ = simpleServiceTube->Zmax-simpleServiceTube->Zmin;
		std::string simpleServiceTubeName = simpleServiceTube->Name;
		double zpos = (simpleServiceTube->Zmin+simpleServiceTube->Zmax)/2;
		
		if(simpleServiceTube->Zmin==0){ //displays services that are single pieces
			if(complexity!=1){
				TGeoVolume *simpleServiceTube_obj =geom->MakeTube(simpleServiceTubeName.c_str(),Al,simpleServiceTube->Rmin,simpleServiceTube->Rmax,dZ);
				simpleServiceTube_obj->SetLineColor(kBlue);
				simpleServiceTube_obj->SetTransparency(75);
				//determine if the simple service belngs in the inner or full assembly
				if(simpleServiceTube->Rmax<215) innerSimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,0));
				SimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,0));
			}
			else{
				TGeoVolume *simpleServiceTube_obj =geom->MakeTube(simpleServiceTubeName.c_str(),Al,simpleServiceTube->Rmin,simpleServiceTube->Rmax,dZ*0.5);
                                simpleServiceTube_obj->SetLineColor(kBlue);
                                simpleServiceTube_obj->SetTransparency(75);
                                //determine if the simple service belngs in the inner or full assembly
                                if(simpleServiceTube->Rmax<215) innerSimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,dZ * 0.5));
                                SimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,dZ * 0.5));
			}
		}

		//display services that have mulitiple parts
		else{
			TGeoVolume *simpleServiceTube_obj =geom->MakeTube(simpleServiceTubeName.c_str(),Al,simpleServiceTube->Rmin,simpleServiceTube->Rmax,dZ/2);
                        simpleServiceTube_obj->SetLineColor(kOrange);
                        simpleServiceTube_obj->SetTransparency(75);
			if(simpleServiceTube->Rmax<215) innerSimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,zpos));
			SimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,zpos));
                        
			if(complexity!=1){
			 //only displays half the detector if complexity is set other than 1 
			//determine if the simple service belongs in the inner or full assembly
			if(simpleServiceTube->Rmax<215) innerSimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,-zpos));
			SimpleServiceAssembly->AddNode(simpleServiceTube_obj,1,new TGeoTranslation(0,0,-zpos));
			}	
		}

	}//end loop over simple services
	
	innerDetector->AddNode(innerSimpleServiceAssembly,1,new TGeoTranslation(0,0,0));
	fullDetector->AddNode(SimpleServiceAssembly,1,new TGeoTranslation(0,0,0));

return siArea;

}
