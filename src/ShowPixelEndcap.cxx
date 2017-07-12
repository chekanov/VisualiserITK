/*Implementation of Pixel Endcap for ITKVisualiser framework
 *
 * @author A.Fischer (LUC)
 */
#include "ShowPixelEndcap.h"
#include <cmath>
#include <iostream>
#include <fstream> 

// implementation of the endcap drawing
vector<double> ShowPixelEndcap::process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoVolume* innerDetector, TGeoVolume* fullDetector, TGeoManager* geom, int complexity,string infile)
{
   vector<double> siAreas;	//surface areas of silicon for modules
   vector<double> ntwoxoneModule;
   vector<double> ntwoxtwoModule;
   vector<double> nonexoneModule;
   
   // define some constants
   const double degree=57.2958;
   const double PI=TMath::Pi();
   const double PI2=2*TMath::Pi();

//get materials from xml reader
   bool hasModuleMat = false;
   bool hasRingMat = false;       //booleans to check if the materials were gathered
   std::vector<MaterialTmp *> materials = reader.getMaterials();
   TGeoMaterial *matModule;
   TGeoMedium *medModule;
   TGeoMaterial *matRing;
   TGeoMedium *medRing; 
   for(int m = 0; m<materials.size();m++){
     MaterialTmp* material = materials.at(m);
     if(material->name=="DefaultPixelModuleMaterial"){
        matModule = new TGeoMaterial("PixelModuleMaterial",0,0,material->density);
        medModule = new TGeoMedium("PixelModuleMedium",1,matModule);
        hasModuleMat = true;
     }
     if(material->name=="DefaultPixelRingMaterial"){
        matRing = new TGeoMaterial("PixelRingMaterial",0,0,material->density);
        medRing = new TGeoMedium("PixelRingMedium",2,matRing);
	hasRingMat = true;
     }
   }

   TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);  //default materials to use in case parser cannot gather specific materials
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
   TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
   TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);
   TGeoMedium *Si = new TGeoMedium("Si",7,7,0,0,0,20,0.1000000,0.00002,0.1000000E-02,1.150551);
	
        //Create assemlblies for inner and outer(inner is used for complexity 3)
	string innerPixelEndcapsName = "InnerPixelEndcaps";
	TGeoVolume *innerPixelEndcaps = new TGeoVolumeAssembly(innerPixelEndcapsName.c_str());
	string PixelEndcapsName = "PixelEndcaps";
        TGeoVolume *PixelEndcaps = new TGeoVolumeAssembly(PixelEndcapsName.c_str());
	

//Build Layers for rings
	std::vector< EndcapLayerTmp *> layers = reader.getPixelEndcapLayers();
	cout<<"about to get Disk supports"<<endl;
	std::vector<EndcapDiskSupportTmp *> diskSupports = reader.getPixelEndcapDiskSupportTemplates();

	unsigned int nlayers = layers.size();
	if(complexity == 3 && nlayers>2) nlayers = 2;

	for(unsigned int i=0; i<nlayers;i++) {
		double siArea = 0;
		double twoxoneModule = 0;
	        double twoxtwoModule =0;
       		double onexoneModule = 0;
		
		EndcapLayerTmp *layer = layers.at(i);
		EndcapDiskSupportTmp *support;
		support = diskSupports.at(diskSupports.size()-i-1);
		cout<<"number of disk supports: "<<diskSupports.size()<<endl;
     		string layername="EndcapLayerAssembly"; 
      		TGeoVolume *assembly_layer = new TGeoVolumeAssembly(layername.c_str());
		
		layer->Print();	

//Build rings in layer
			
		int nRings = layer->ringpos.size();
			
 		for(int iR=0;iR<nRings;++iR){

			string ringname = "EndcapRingAssembly";
               		TGeoVolume *assembly_rings = new TGeoVolumeAssembly(ringname.c_str());
			double ringposition = layer->ringpos[iR];
			double innerradius;
			double outerradius;
			string modType = layer->modtype[0];
			double totalRingThickness = layer->thickness[0];
			double supportInnerRadius = support->rmin[iR];
			double supportOuterRadius = support->rmax[iR];
			double supportThickness = support->thickness;
		
			if(layer->innerRadius[1]==0) innerradius=layer->innerRadius[0];
			else innerradius=layer->innerRadius[iR];
	
			if(layer->outerRadius[1]==0) outerradius=layer->outerRadius[0];
                        else outerradius=layer->outerRadius[iR];

			if(layer->modtype[1]==0) modType = layer->modtype[0];
			else modType = layer->modtype[iR];

			if(layer->thickness[1]!=0) totalRingThickness=layer->thickness[iR];
			if(support->rmin[1]<=1||support->rmin[1]>3000000) supportInnerRadius=support->rmin[0]; 
			if(support->rmax[1]<=1||support->rmax[1]>3000000) supportOuterRadius=support->rmax[0];
			
			cout<<"Disk Support number/inner radius / outer radius"<<iR<<" / "<<supportInnerRadius<<" / "<<supportOuterRadius<<endl;	
			string halfringname = "EndcapHalfRingSupport";
			TGeoVolume *ring_obj = geom->MakeTubs(halfringname.c_str(),medRing,supportInnerRadius,supportOuterRadius,supportThickness,-1*PI*degree/2,PI*degree/2);
			ring_obj->SetLineColor(kBlack);
			ring_obj->SetTransparency(65);	

			TGeoVolume *ring_obj_mirror = geom->MakeTubs(halfringname.c_str(),medRing,supportInnerRadius,supportOuterRadius,supportThickness,-1*PI*degree/2,PI*degree/2);
                        ring_obj_mirror->SetLineColor(kBlack);
                        ring_obj_mirror->SetTransparency(65);

						
			TGeoRotation *rot = new TGeoRotation();
			rot->RotateZ(180);		//sets up rotation for other half of endcap 
			
					
			//-------------Sectors added here
			if(complexity!=2){
			int nSectors;  
			double angleSector;
			
			if(layer->nsectors[1]==0){
				nSectors = layer->nsectors[0];
				angleSector =(PI*degree)/nSectors;

			}
			else{
				nSectors = layer->nsectors[iR];
				angleSector =(PI*degree)/nSectors;	//the arc angle taken up by each sector
			}
			
			//Build Sectors(areas that hold modules)
			for(int iS = 0; iS<nSectors; ++iS){
				ModuleTmp *moduleTmp = reader.getModuleTemplate(modType);
				double phiOfMod0 = layer->phioffset[0];
                                if(phiOfMod0 == 0) phiOfMod0 = PI/2;
				//paramaters that define the 4 different types of modules can be cleaned up (very messy right now)
				double ringModuleAngle =iS * angleSector - (degree * phiOfMod0);
				double ringModuleAngleMirror =-iS * angleSector + (degree * phiOfMod0);
				double ringModuleRadius = innerradius + moduleTmp->length/2;
				double xPos = ringModuleRadius * cos(ringModuleAngle/degree);
				double yPos = ringModuleRadius * sin(ringModuleAngle/degree);
				double xPosShift = ringModuleRadius * cos((ringModuleAngle+angleSector)/degree);
				double yPosShift = ringModuleRadius * sin((ringModuleAngle+angleSector)/degree);
				double xPosMirror = ringModuleRadius * cos(ringModuleAngleMirror/degree);
                                double yPosMirror = ringModuleRadius * sin(ringModuleAngleMirror/degree);
                                double xPosShiftMirror = ringModuleRadius * cos((ringModuleAngleMirror-angleSector)/degree);
                                double yPosShiftMirror = ringModuleRadius * sin((ringModuleAngleMirror-angleSector)/degree);
				double nChips = 0;
                                double areaChips = 0;		
				double deadArea = 0;
				int nPixels = 0; //for calculating siarea using the pixel count method

				cout<<"ring module angle: "<<ringModuleAngle<<" half angle between modules: "<<angleSector<<endl;
				TGeoRotation *rotModule = new TGeoRotation();	

				TGeoVolume *module = geom->MakeBox(modType.c_str(),medModule,moduleTmp->widthmax / 2, moduleTmp->length / 2, moduleTmp->thickness / 2);
				module->SetLineColor(4);
				module->SetTransparency(60);
				
				//add modules to the rings
				if (iS%4 == 0){
					rotModule->RotateZ(ringModuleAngle);
					ring_obj->AddNode(module,iS+1,new TGeoCombiTrans(xPos, yPos, (layer->zoffset[0]),rotModule));
				}
				 //can be updated to allow for varying zoffsets among rings
				if (iS%4 == 1){
					rotModule->RotateZ(ringModuleAngle+angleSector);
					ring_obj->AddNode(module,iS+1,new TGeoCombiTrans(xPosShift, yPosShift, -(layer->zoffset[0]),rotModule));
				}	
				if(layer->splitOffSet!=0){
					if (iS%4 == 2){
						rotModule->RotateZ(ringModuleAngleMirror);
			 			ring_obj_mirror->AddNode(module,iS+1,new TGeoCombiTrans(xPosMirror, yPosMirror, (layer->zoffset[0]),rotModule));
					}
					if (iS%4 == 3){
				 		rotModule->RotateZ(ringModuleAngleMirror-angleSector);
						ring_obj_mirror->AddNode(module,iS+1,new TGeoCombiTrans(xPosShiftMirror, yPosShiftMirror,- (layer->zoffset[0]),rotModule));		
					}
				}
				
				//---------------calculates Silicon surface area and tallies up module types
				nChips = moduleTmp->lengthChips * moduleTmp->widthMaxChips;				
				//the number of modules is dependent on the number of times the halfring is copied
				if(complexity != 1) {
					if(nChips == 1) {
						onexoneModule+=2;
						deadArea = 7.88*2;
					}
	                                if(nChips == 2) {
						twoxoneModule+=2;
						deadArea = 15.76*2; //same dead are for double length as dobule width
					}
        	                        if(nChips == 4) {
						twoxtwoModule+=2;
						deadArea = 31.52*2;
					}
					nChips = nChips * 2; //the second half of the detector (-z region)
					
				}
				else{
					if(nChips == 1) {
						onexoneModule++;
						deadArea = 7.88;
					}
                                        if(nChips == 2){
						twoxoneModule++;
						deadArea = 15.76;//doesnt matter if double length or width
                                        }
                                        if(nChips == 4){
						twoxtwoModule++;
						deadArea = 31.52;
					} 
				}
				
				//each half ring is half a ring and is double sided
				areaChips = reader.getChipTemplate(moduleTmp->chip_type)->length * reader.getChipTemplate(moduleTmp->chip_type)->width;
        			//siArea += (areaChips * nChips)+deadArea;
        			nPixels=(reader.getChipTemplate(moduleTmp->chip_type)->rows * reader.getChipTemplate(moduleTmp->chip_type)->columns);
				siArea += nPixels* nChips * (reader.getChipTemplate(moduleTmp->chip_type)->pitchPhi * reader.getChipTemplate(moduleTmp->chip_type)->pitchEta)-2*20;
				}//end loop over Modules
				
			}//end if complexity!=2 (ie. no modules)
			
			//------adds rings to assemblies
			assembly_rings->AddNode(ring_obj,iR+1,new TGeoCombiTrans(0,0,ringposition+(layer->splitOffSet),0));
			if(layer->splitOffSet!=0) assembly_rings->AddNode(ring_obj_mirror,iR+1,new TGeoCombiTrans(0,0,ringposition-(layer->splitOffSet),rot));
			else assembly_rings->AddNode(ring_obj,iR+1,new TGeoCombiTrans(0,0,ringposition,rot));
			//halfrings are shifted away to make room for servicies

			//add the second half of the detector in the -z area
			if(complexity!=1){ 
                        assembly_rings->AddNode(ring_obj,iR+1+nRings,new TGeoCombiTrans(0,0,-ringposition+(layer->splitOffSet),0));
			if(layer->splitOffSet!=0) assembly_rings->AddNode(ring_obj_mirror,iR+1+nRings,new TGeoCombiTrans(0,0,-ringposition-(layer->splitOffSet),rot));
			else assembly_rings->AddNode(ring_obj,iR+1+nRings,new TGeoCombiTrans(0,0,-ringposition,rot));
                       	//^^^^adds the other side of the detector as a mirror provided the user set complexity at an appropriate value
                        }

			assembly_layer->AddNode(assembly_rings,iR+1,new TGeoTranslation(0,0,0));
			
		}//end loop over rings
	
		 nonexoneModule.push_back(onexoneModule);
                 ntwoxoneModule.push_back(twoxoneModule);
                 ntwoxtwoModule.push_back(twoxtwoModule);
	
		//puts the layers in the correct inner or full assembly
		if(i<2) innerPixelEndcaps->AddNode(assembly_layer, i+1, new TGeoTranslation(0,0,0));
		PixelEndcaps->AddNode(assembly_layer, i+1, new TGeoTranslation(0,0,0));
			
   		siAreas.push_back(siArea);
	}//end loop over layers

   //add the Endcap assemblies to the inner or outer detector to be used in itkvis.cxx
   innerDetector->AddNode(innerPixelEndcaps,1,new TGeoTranslation(0,0,0));
   fullDetector->AddNode(PixelEndcaps,1,new TGeoTranslation(0,0,0));
	
   //si Area text file output
   ofstream ofsSiArea;
   string style = to_string(complexity);
   string out = "out/"+infile+"_SiArea_"+style+".txt";
   double totonexoneModules = 0;
   double tottwoxoneModules = 0;
   double tottwoxtwoModules = 0;

   ofsSiArea.open(out);
   if(complexity!=0) ofsSiArea<<"Warning you are not viewing the full detector, switch to the full detector(style 0) to find siArea info for the entire layout"<<endl;
   ofsSiArea<<"Si Area calulated by summing pixel areas (19.2mm x 20mm) per chip, and subtracting periphery region(2mm x 20mm) per chip. You are viewing: "<<infile<<" with style: "<<complexity<<endl<<"Endcaps"<<endl;

   ofsSiArea<<"layer:    Number of 1x1 Modules:"<<endl;
   for(int i = 0; i<nonexoneModule.size();i++){
     ofsSiArea<<"    "<<i<<" has: "<<nonexoneModule[i]<<endl;
     totonexoneModules+=nonexoneModule[i];
   }

   ofsSiArea<<"layer:    Number of 2x1 Modules:"<<endl;
   for(int i = 0; i<ntwoxoneModule.size();i++){
     ofsSiArea<<"    "<<i<<" has: "<<ntwoxoneModule[i]<<endl;
     tottwoxoneModules+=ntwoxoneModule[i];
   }

   ofsSiArea<<"layer:    Number of 2x2 Modules:"<<endl;
   for(int i = 0; i<ntwoxtwoModule.size();i++){
     ofsSiArea<<"    "<<i<<" has: "<<ntwoxtwoModule[i]<<endl;
     tottwoxtwoModules+=ntwoxtwoModule[i];
   }
  ofsSiArea<<"The Endcaps have a total of: "<<totonexoneModules<<" 1x1 Modules, "<<tottwoxoneModules<<" 2x1 Modules, and "<<tottwoxtwoModules<<" 2x2 Modules"<<endl<<endl;
  ofsSiArea.close();

  return siAreas;
} 


