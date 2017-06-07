#include "ShowPixelEndcap.h"

// implementation of the endcap drawing
bool ShowPixelEndcap::process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoManager* geom)
{


   // define some constants
   const double degree=57.2958;
   const float PI=TMath::Pi();
   const float PI2=2*TMath::Pi();


//--- define some materials
   TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
   TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
   TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);
   TGeoMedium *Si = new TGeoMedium("Si",7,7,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);

//Build Layers for discs
	std::vector< EndcapLayerTmp *> layers = reader.getPixelEndcapLayers();

	for(unsigned int i=0; i<layers.size();i++) {
		EndcapLayerTmp *layer = layers.at(i);
     		string layername="LayerAssembly"; // +string(layer->name);
      		TGeoVolume *assembly_layer = new TGeoVolumeAssembly(layername.c_str());
    		//double spreadDistance = (layer->splitOffset)/2;
		

		layer->Print();	
//Build rings  in layer
		string ringname = "RingAssembly";
		TGeoVolume *assembly_rings = new TGeoVolumeAssembly(ringname.c_str());
	
		int nRings = layer->ringpos.size();
		
		
 		for(int iR=0;iR<nRings;++iR){
			double ringposition = layer->ringpos[iR];
			TGeoVolume *ring_obj = geom->MakeTubs(ringname.c_str(),Al,layer->innerRadius[0],layer->outerRadius[0],layer->thickness[0],-1*PI*degree/2,PI*degree/2);
			ring_obj->SetLineColor(kBlue);
			ring_obj->SetTransparency(70);	
			assembly_rings->AddNode(ring_obj,iR+1,new TGeoTranslation(5,0,ringposition));//not sure if this puts the half ring in the right position yet

			TGeoRotation *rot = new TGeoRotation();
			rot->RotateZ(180);		//sets up rotation for other half of endcap 
			
			assembly_rings->AddNode(ring_obj,iR+1+nRings,new TGeoCombiTrans(-5,0,ringposition,rot));//need to add splitDistance to x value of translation 5 is a placeholder value
		}
	assembly_layer->AddNode(assembly_rings,i+1,new TGeoTranslation(0,0,0));
	
	top->AddNode(assembly_layer, i+1, new TGeoTranslation(0,0,0));
	}
		
	




   return true;
} 


