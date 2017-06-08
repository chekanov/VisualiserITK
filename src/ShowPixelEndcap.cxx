#include "ShowPixelEndcap.h"

// implementation of the endcap drawing
bool ShowPixelEndcap::process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoManager* geom)
{


   // define some constants
   const double degree=57.2958;
   const double PI=TMath::Pi();
   const double PI2=2*TMath::Pi();


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
     		string layername="EndcapLayerAssembly"; 
      		TGeoVolume *assembly_layer = new TGeoVolumeAssembly(layername.c_str());
    		//double spreadDistance = (layer->splitOffset)/2;
		

		layer->Print();	
//Build rings  in layer
//Build rings in layer
	
		int nRings = layer->ringpos.size();
		
 		for(int iR=0;iR<nRings;++iR){
		
			string ringname = "EndcapRingAssembly";
               		TGeoVolume *assembly_rings = new TGeoVolumeAssembly(ringname.c_str());
			
			string halfringname = "EndcapHalfRing";
			double ringposition = layer->ringpos[iR];
			double innerradius;
			double outerradius;

			if(layer->innerRadius[1]==0){
				innerradius=layer->innerRadius[0];
			}
			else{
				innerradius=layer->innerRadius[iR];
			}
			
			if(layer->outerRadius[1]==0){
                                outerradius=layer->outerRadius[0];
                        }
                        else{
                                outerradius=layer->outerRadius[iR];
                        }

			TGeoVolume *ring_obj = geom->MakeTubs(halfringname.c_str(),Al,innerradius,outerradius,layer->thickness[0],-1*PI*degree/2,PI*degree/2);
			//assumes the radii and thickness are constant for the layer so far xml files follow this aswell
			ring_obj->SetLineColor(kBlue);
			ring_obj->SetTransparency(70);	
			
			assembly_rings->AddNode(ring_obj,iR+1,new TGeoCombiTrans(5,0,ringposition,0));	

			TGeoRotation *rot = new TGeoRotation();
			rot->RotateZ(180);		//sets up rotation for other half of endcap 
			
					
			//-------------Sectors added here
			int nSectors;
			double angleSector;
			if(layer->nsectors[1]==0){
				nSectors = layer->nsectors[0];
				angleSector =(PI2*degree)/nSectors;

		}
			else{
				nSectors = layer->nsectors[iR];
				angleSector =(PI2*degree)/nSectors;	//the angle of the arc taken up by each sector
			}
			//Build Sectors
			string sectorname = "RingSector";
			
			for(int iS = 0; iS<nSectors/2;++iS){
				TGeoVolume *sect_obj = geom->MakeTubs(sectorname.c_str(),Al,innerradius,outerradius,layer->thickness[0],(-1*PI*degree)/2+iS*angleSector,(-1*PI*degree)/2+iS*angleSector+angleSector);
				sect_obj->SetLineColor(kBlue);
				sect_obj->SetTransparency(70);
				if(layer->double_sided)	//needs to be tested with an xml file that has double sided endcaps
					sect_obj =sect_obj->Divide("Modules",3,2,1,1);
				ring_obj->AddNode(sect_obj,iS+1,new TGeoTranslation(0,0,0));
			}
			assembly_rings->AddNode(ring_obj,iR+1,new TGeoCombiTrans(-5,0,ringposition,rot));
                        //need to add splitDistance to x value of translation 5 is a placeholder value

                        assembly_rings->AddNode(ring_obj,iR+1+nRings,new TGeoCombiTrans(5,0,-ringposition,0));
                        assembly_rings->AddNode(ring_obj,iR+1+nRings,new TGeoCombiTrans(-5,0,-ringposition,rot));
                       	//adds the other side of the detector as a mirror
                        
			assembly_layer->AddNode(assembly_rings,iR+1,new TGeoTranslation(0,0,0));
			
		}
	
	top->AddNode(assembly_layer, i+1, new TGeoTranslation(0,0,0));
	}
		
	




   return true;
} 


