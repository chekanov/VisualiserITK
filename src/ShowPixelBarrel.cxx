/* Visualization of Barrel Pixel  
*
* @author S.Chekanov (ANL) 
*/
#include "ShowPixelBarrel.h"

vector<double> ShowPixelBarrel::process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoVolume* innerDetector, TGeoVolume* fullDetector, TGeoManager* geom, int complexity, string infile)
{

   vector<double> siAreas;
   vector<double> ntwoxoneModule;
   vector<double> ntwoxtwoModule;
   vector<double> nonexoneModule;

   // define some constants
   const float degree=57.2958;
   const float PI=TMath::Pi();
   const float PI2=2*TMath::Pi();


//--- define some default materials
   TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
   TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
   TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);
   TGeoMedium *Si = new TGeoMedium("Si",7,7,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);

// get specified materials
   std::vector<MaterialTmp *> materials = reader.getMaterials();
   TGeoMaterial *matModule;
   TGeoMedium *medModule;
   TGeoMaterial *matStave;
   TGeoMedium *medStave;
   for(int m = 0; m<materials.size();m++){
     MaterialTmp* material = materials.at(m);
     if(material->name=="DefaultPixelModuleMaterial"){
        matModule = new TGeoMaterial("PixelModuleMaterial",0,0,material->density);
        medModule = new TGeoMedium("PixelModuleMedium",1,matModule);
     }
     if(material->name=="DefaultPixelStaveMaterial"){
        matStave = new TGeoMaterial("PixelStaveMaterial",0,0,material->density);
        medStave = new TGeoMedium("PixelStaveMedium",2,matStave);
     }
   }  
   //Decalare Some assemblies to hold volumes, inner is used for complexity 3(inner 2 layer)
   string innerPixelBarrelName = "InnerPixelBarrel";
   TGeoVolume *innerPixelBarrel = new TGeoVolumeAssembly(innerPixelBarrelName.c_str());
   string PixelBarrelName = "PixelBarrel";
   TGeoVolume *PixelBarrel = new TGeoVolumeAssembly(PixelBarrelName.c_str());

   string innerPixelBarrelRingsName = "InnerPixelBarrelRings";
   TGeoVolume *innerPixelBarrelRings = new TGeoVolumeAssembly(innerPixelBarrelRingsName.c_str());
   string PixelBarrelRingsName = "PixelBarrelRings";
   TGeoVolume *PixelBarrelRings = new TGeoVolumeAssembly(PixelBarrelRingsName.c_str());


    // look at layers and build them
    std::vector< BarrelLayerTmp *> layers = reader.getPixelBarrelLayers();
    unsigned int nlayers = layers.size();
    if(complexity == 3 && nlayers>2) nlayers = 2;

    for(unsigned int i=0; i<nlayers;i++) {
      BarrelLayerTmp *layer = layers.at(i);
      string layername="LayerAssembly"; // +string(layer->name);
      TGeoVolume *assembly_layer = new TGeoVolumeAssembly(layername.c_str());

      double layer_radius=layer->radius;
      double siArea = 0;      
      double twoxoneModule = 0;
      double twoxtwoModule =0;
      double onexoneModule = 0;      

      string ringLayerName="RingLayerAssembly"; // +string(layer->name);
      TGeoVolume *ringLayer_Assembly = new TGeoVolumeAssembly(ringLayerName.c_str());



      cout << "\n\n -->  INFO for layer = " << layer->index << endl;
      layer->Print();

     cout << "       \n\n --> LOOK at stave for the layer = " << layer->index << endl;
     // look at staves
     std::vector<StaveTmp*> staves=reader.getPixelStaveTemplate(layer->index);
     int n_staves=layer->stave_n;
     cout << "       \n\n --> Nr of staves for this layer = " << n_staves <<  " (or vector size) = "  << staves.size()<< endl;


    double  sector_phi =  (2*TMath::Pi() / n_staves);

     float layer_halfplainlength=0;

     // loop over all staves..
     for(int ist=0; ist<n_staves; ist++){

      int istave=0;
      StaveTmp *stave =  staves.at(istave);
      string stave_type=stave->type;

      //  compute the stave length and steps for *tracking*
      double halfplainlength = reader.getHalfPlainLength(stave);
      double zstep = reader.getZStep(stave);
      layer_halfplainlength=halfplainlength;
      if (complexity == 2) layer_halfplainlength*=0.5; //only want half a barrel
      float   stave_ang=-1*stave->b_tilt*degree;
      if(stave->b_tilt == 0) stave_ang = 0.24435*degree; //default stave tilt angle if none is given
      // cout << "barel module rotation=" << stave_ang << endl;


      // look at the I-beam stave width calculated form module width 
      double stave_width = reader.getStaveWidth(stave);
      // cout << "######## Module width=" << module_width << endl;
      double stave_thickness=2; // which is correct number ?? 
 
     

      string stavename="Stave ("+stave_type+")"; // std::to_string(ist+1);
      TGeoVolume *assembly_stave = new TGeoVolumeAssembly(stavename.c_str());
	
      string barrelringname="Barrel Rings";
      TGeoVolume *assembly_barrelring = new TGeoVolumeAssembly(barrelringname.c_str());

      stavename="Support ("+stave_type+")"; // +std::to_string(istave+1);
      //create the stave support
      TGeoVolume *stave_obj;
      if (complexity == 1) stave_obj = geom->MakeTrd1(stavename.c_str(), medStave, 0.5*stave_width, 0.00000000001 ,0.5*layer_halfplainlength, 0.5*stave_thickness);
      else stave_obj = geom->MakeTrd1(stavename.c_str(), medStave, 0.5*stave_width, 0.00000000001 , layer_halfplainlength, 0.5*stave_thickness);
      stave_obj->SetLineColor(kRed);
      stave_obj->SetTransparency(70);  //root.cern.ch/doc/v608/classTGeoTranslation.html ;
     // rotate stave
     //TGeoTranslation * translate_stave = new TGeoTranslation(ll,0,0); 
      TGeoRotation * rot = new TGeoRotation(); 
      rot->SetAngles(90, 90, 0); 
      if ( complexity ==1) assembly_stave->AddNode(stave_obj,ist+1 , new TGeoCombiTrans(0,0, 0.5*layer_halfplainlength ,rot));
      else assembly_stave->AddNode(stave_obj,ist+1 , new TGeoCombiTrans(0,0,0,rot));
    
     //stave "mountain" modules aka. barrel ring modules
     if(stave_type.compare("Alpine")==0&&complexity!=2){
	cout<<"about to read alpine module templates stavetype: "<<stave_type<<endl;
        InDet::ModuleTmp* ringModule = reader.getModuleTemplate(stave->alp_type);
        std::vector<double> rmodule_pos = stave->alp_pos;		
        double rmodule_length = ringModule->length;
        double rmodule_width  = ringModule->widthmax;
        double rmodule_thickness  = ringModule->thickness;
        string rmodule_name= ringModule->name;
        string rmodule_chip =ringModule->chip_type;
        double rmodule_angle = stave->alp_angle;
        double rmodule_rshift = stave->alp_rshift;
        cout << " ->Ring Module name=" << rmodule_name << " Chip=" << rmodule_chip << " length=" << rmodule_length << " mm  width=" << rmodule_width << " mm" << endl;
     
     //add Barrel ring modules
     for(int irmod=0;irmod<rmodule_pos.size();irmod++){
	double nChips = 0;
	double areaChips = 0;
	double deadArea = 0;
	int nPixels = 0;	

	//siArea info and module counts
	nChips = ringModule->lengthChips * ringModule->widthMaxChips;

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

	//areaChips = reader.getChipTemplate(rmodule_chip)->length * reader.getChipTemplate(rmodule_chip)->width;
	//siArea += (areaChips * nChips)+deadArea;
	
	nPixels=(reader.getChipTemplate(ringModule->chip_type)->rows * reader.getChipTemplate(ringModule->chip_type)->columns);
        siArea += nPixels * nChips * (reader.getChipTemplate(ringModule->chip_type)->pitchPhi * reader.getChipTemplate(ringModule->chip_type)->pitchEta)-2*20;
	//end siArea info section

	TGeoVolume * rmodule_obj =  geom->MakeBox(rmodule_name.c_str(),medModule,rmodule_thickness,0.5*rmodule_width,0.5*rmodule_length);
        rmodule_obj->SetLineColor(kTeal+3);
        rmodule_obj->SetTransparency(65);
	
	TGeoRotation *ringRot = new TGeoRotation;
	ringRot->RotateY(-rmodule_angle*degree);
	TGeoRotation *ringRotFlipped = new TGeoRotation;
        ringRotFlipped->RotateY(rmodule_angle*degree);

	assembly_barrelring->AddNode(rmodule_obj,irmod+1,new TGeoCombiTrans(0,0,rmodule_pos[irmod],ringRot));
	if(complexity!=1){
	//only displays half the detector if the complexity is set to 1
	assembly_barrelring->AddNode(rmodule_obj,irmod+1,new TGeoCombiTrans(0,0,-rmodule_pos[irmod],ringRotFlipped));
	}

	float  ringxpos=(layer_radius-stave_thickness+rmodule_rshift)*TMath::Cos(sector_phi*ist);
        float ringypos=(layer_radius-stave_thickness+rmodule_rshift)*TMath::Sin(sector_phi*ist);

	TGeoRotation *rotBarrelRings=new TGeoRotation;
        rotBarrelRings->SetAngles(sector_phi*ist*degree,0,0);
	
	ringLayer_Assembly->AddNode(assembly_barrelring,ist+1, new TGeoCombiTrans(ringxpos,ringypos,0,rotBarrelRings));
	}
     }

     // show one pixel module
      InDet::ModuleTmp* plainModule = reader.getModuleTemplate(stave->b_type);
      double module_length = plainModule->length;
      double module_width  = plainModule->widthmax;
      double module_thickness  = plainModule->thickness;
      double moduleplain_gap = stave->b_gap;
      string module_name= plainModule->name;
      string module_chip =plainModule->chip_type;
      cout << " -> Module name=" << module_name << " Chip=" << module_chip << " length=" << module_length << " mm  width=" << module_width << " mm" << endl;


// add modules if complexity is set to show them
      if(complexity!=2){
      int nmodplain = stave->b_modn;
      int nmodtrans = 2*stave->trans_pos.size();
      int NMOD = nmodtrans + nmodplain;
      if (complexity==1) NMOD*=0.5;
      // add modules
      
      for (int imod=0; imod<NMOD; imod++) {
	double nChips = 0;
        double areaChips = 0;
        double deadArea = 0;
	int nPixels = 0;
	float zpos;
	//siArea info and module counts
        nChips = plainModule->lengthChips * plainModule->widthMaxChips;
		
	if(nChips == 1) {
           onexoneModule++;
           deadArea = 7.88;
        }
        if(nChips == 2){
           twoxoneModule++;
	   deadArea = 15.76;
        }
        if(nChips == 4){
           twoxtwoModule++;
           deadArea = 31.52;
        }
	
        //areaChips = reader.getChipTemplate(plainModule->chip_type)->length * reader.getChipTemplate(plainModule->chip_type)->width;
        //siArea += areaChips * nChips+deadArea;

	nPixels=(reader.getChipTemplate(plainModule->chip_type)->rows * reader.getChipTemplate(plainModule->chip_type)->columns);
        siArea += nPixels * nChips * (reader.getChipTemplate(plainModule->chip_type)->pitchPhi * reader.getChipTemplate(plainModule->chip_type)->pitchEta)-2*20;

        TGeoVolume * module_obj =  geom->MakeBox(module_name.c_str(),medModule,0.5*module_thickness,0.5*module_width,0.5*module_length);
        module_obj->SetLineColor(80);
        module_obj->SetFillColor(80);
        module_obj->SetTransparency(70);

        // shift all by 1 pixel 
       if(complexity != 1) zpos=-halfplainlength+(0.5*(module_length+moduleplain_gap)) +  imod*(module_length+moduleplain_gap);
       else zpos=(0.5*(module_length+moduleplain_gap)) + imod*(module_length+moduleplain_gap);	
        // comment this out to speed up
        // shift since sits on top of stave
         assembly_stave->AddNode(module_obj, imod+stave_thickness, new TGeoCombiTrans((-stave_thickness-module_thickness)*0.5, 0, zpos, 0));

      } // end loop over modules 
    


      float xpos=(layer_radius)*TMath::Cos(sector_phi*ist);
      float ypos=(layer_radius)*TMath::Sin(sector_phi*ist);
      

      TGeoRotation * rotstave = new TGeoRotation();

      float rotation_degree=stave_ang+ sector_phi*ist*degree;
      rotstave->SetAngles(rotation_degree, 0, 0); // all angles in degrees
      if (i == 1 || i == 3) rotstave->SetAngles(rotation_degree+180,0,0);
          
      if (complexity ==2) assembly_layer->AddNode(assembly_stave,ist+1, new TGeoCombiTrans(xpos,ypos,layer_halfplainlength * 0.25,rotstave));
      else assembly_layer->AddNode(assembly_stave,ist+1, new TGeoCombiTrans(xpos,ypos,0,rotstave));
      stave->Print();

     }; // end loop over staves 
    }
 	/*//support rings for barrel
 	string supportRingName = "BarrelSupportRing";
	TGeoVolume *supportRing_obj =geom->MakeTube(supportRingName.c_str(),Al,layer_radius-10,layer_radius+10,10);
	//placehold untill support ring thickness can be determined
	supportRing_obj->SetLineColor(kRed);
	supportRing_obj->SetTransparency(65);
	
	pixel_barrel->AddNode(supportRing_obj,1,new TGeoTranslation(0,0,layer_halfplainlength+10));
	//1.5offset is a placeholder till xml files can be deterimned
	pixel_barrel->AddNode(supportRing_obj,2,new TGeoTranslation(0,0,-layer_halfplainlength-10));	
	*/
   
     cout << "Layer Nr " << layer->index << " HalfPlainLength=" << layer_halfplainlength << endl;
//      Rmin, Rmax, and a half-length dZ=1cm :
      string fullname="PixelBarrelLayer"; // string(layer->name)+std::to_string(i);
      TGeoVolume *LAYER;
      if (complexity ==1) LAYER = geom->MakeTube(fullname.c_str(), Al, layer_radius, layer_radius, layer_halfplainlength*0.5);
      else LAYER = geom->MakeTube(fullname.c_str(), Al, layer_radius, layer_radius, layer_halfplainlength);
      LAYER->SetLineColor(kBlue);
      LAYER->SetTransparency(90);
      fullname="Layer"; //  + std::to_string(i);
      if (complexity == 1) assembly_layer->AddNode(LAYER, i+1, new TGeoTranslation(0,0,layer_halfplainlength * 0.5));
      else assembly_layer->AddNode(LAYER, i+1, new TGeoTranslation(0,0,0));

//    add the layers to the appropriate assemblies
      if(i<2)innerPixelBarrel->AddNode(assembly_layer, i+1, new TGeoTranslation(0,0,0));
      PixelBarrel->AddNode(assembly_layer, i+1, new TGeoTranslation(0,0,0));
      if(i<2) innerPixelBarrelRings->AddNode(ringLayer_Assembly,i+1,new TGeoTranslation(0,0,0));
      PixelBarrelRings->AddNode(ringLayer_Assembly,i+1,new TGeoTranslation(0,0,0));

      siAreas.push_back(siArea);
      nonexoneModule.push_back(onexoneModule);
      ntwoxoneModule.push_back(twoxoneModule);
      ntwoxtwoModule.push_back(twoxtwoModule);
       
    }
	//add the assemblies to the inner and full detector assemblies
   innerDetector->AddNode(innerPixelBarrel,1,new TGeoTranslation(0,0,0));
   innerDetector->AddNode(innerPixelBarrelRings,1,new TGeoTranslation(0,0,0));
   fullDetector->AddNode(PixelBarrel,1,new TGeoTranslation(0,0,0));
   fullDetector->AddNode(PixelBarrelRings,1,new TGeoTranslation(0,0,0));

   //Si Area and module count file writing-----------------------
	
   ofstream ofsSiArea;
   string style = to_string(complexity);
   string out = "out/"+infile+"_SiArea_"+style+".txt";
   double totonexoneModules = 0;
   double tottwoxoneModules = 0;
   double tottwoxtwoModules = 0;

   ofsSiArea.open(out,ios::app);
   ofsSiArea<<"Barrel and Barrel rings: "<<endl;
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
  ofsSiArea<<"The Barrel has a total of: "<<totonexoneModules<<" 1x1 Modules, "<<tottwoxoneModules<<" 2x1 Modules, and "<<tottwoxtwoModules<<" 2x2 Modules"<<endl<<endl;
  ofsSiArea.close();

return siAreas;
} 



