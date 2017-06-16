/* Visualization of Barrel Pixel  
*
* @author S.Chekanov (ANL) 
*/
#include "ShowPixelBarrel.h"

bool ShowPixelBarrel::process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoManager* geom)
{



   // define some constants
   const float degree=57.2958;
   const float PI=TMath::Pi();
   const float PI2=2*TMath::Pi();


//--- define some materials
   TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
   TGeoMaterial *matAl = new TGeoMaterial("Al", 26.98,13,2.7);
//   //--- define some media
   TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
   TGeoMedium *Al = new TGeoMedium("Aluminium",2, matAl);
   TGeoMedium *Si = new TGeoMedium("Si",7,7,0,0,0,20,0.1000000E+11,0.212,0.1000000E-02,1.150551);

   string pixelBarrelName = "PixelBarrel";
   TGeoVolume *pixel_barrel = new TGeoVolumeAssembly(pixelBarrelName.c_str());
   
   string pixelBarrelRingsName = "PixelBarrelRings";
   TGeoVolume *pixelBarrelRings_Assembly = new TGeoVolumeAssembly(pixelBarrelRingsName.c_str());

    // look at layers and build them
    std::vector< BarrelLayerTmp *> layers = reader.getPixelBarrelLayers();
    for(unsigned int i=0; i<layers.size();i++) {
      BarrelLayerTmp *layer = layers.at(i);
      string layername="LayerAssembly"; // +string(layer->name);
      TGeoVolume *assembly_layer = new TGeoVolumeAssembly(layername.c_str());
      double layer_radius=layer->radius;
	
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
      float   stave_ang=-1*stave->b_tilt*degree;
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
      TGeoVolume *stave_obj = geom->MakeBox(stavename.c_str(), Al, stave_thickness, 0.5*stave_width, stave->support_halflength);
      stave_obj->SetLineColor(kRed);
      stave_obj->SetTransparency(70);  //root.cern.ch/doc/v608/classTGeoTranslation.html ;
     // rotate stave
      //TGeoTranslation * translate_stave = new TGeoTranslation(ll,0,0); 
      TGeoRotation * rot = new TGeoRotation(); 
      rot->SetAngles(0, 0, 0); 
      assembly_stave->AddNode(stave_obj,ist+1 , new TGeoCombiTrans(0,0,0,rot));
    
     //stave "mountain" modules aka. barrel ring modules
     if(stave_type.compare("Alpine")==0){
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
	TGeoVolume * rmodule_obj =  geom->MakeBox(rmodule_name.c_str(),Si,rmodule_thickness,0.5*rmodule_length,0.5*rmodule_width);
        rmodule_obj->SetLineColor(80);
        rmodule_obj->SetFillColor(80);
        rmodule_obj->SetTransparency(70);
	
	TGeoRotation *ringRot = new TGeoRotation;
	ringRot->RotateY(-rmodule_angle*degree);
	TGeoRotation *ringRotFlipped = new TGeoRotation;
        ringRotFlipped->RotateY(rmodule_angle*degree);


	assembly_barrelring->AddNode(rmodule_obj,irmod+1,new TGeoCombiTrans(0,0,rmodule_pos[irmod],ringRot));
	assembly_barrelring->AddNode(rmodule_obj,irmod+1,new TGeoCombiTrans(0,0,-rmodule_pos[irmod],ringRotFlipped));
	
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


// add modules
      int nmodplain = stave->b_modn;
      int nmodtrans = 2*stave->trans_pos.size();
      int NMOD = nmodtrans + nmodplain;
      // add modules 
      for (int imod=0; imod<NMOD; imod++) {
        TGeoVolume * module_obj =  geom->MakeBox(module_name.c_str(),Si,module_thickness,0.5*module_width,0.5*module_length);
        module_obj->SetLineColor(80);
        module_obj->SetFillColor(80);
        module_obj->SetTransparency(70);

        // shift all by 1 pixel 
        float zpos=-halfplainlength+(0.5*(module_length+moduleplain_gap)) +  imod*(module_length+moduleplain_gap);

        // comment this out to speed up
        // shift +1 mm since sits on top of stave
         assembly_stave->AddNode(module_obj, imod+stave_thickness, new TGeoCombiTrans(1,0,zpos,rot));

      } // end loop over modules 
    


      float xpos=(layer_radius-stave_thickness)*TMath::Cos(sector_phi*ist);
      float ypos=(layer_radius-stave_thickness)*TMath::Sin(sector_phi*ist);
      

      TGeoRotation * rotstave = new TGeoRotation();

      float rotation_degree=stave_ang+ sector_phi*ist*degree;
      rotstave->SetAngles(rotation_degree, 0, 0); // all angles in degrees
                
      assembly_layer->AddNode(assembly_stave,ist+1, new TGeoCombiTrans(xpos,ypos,0,rotstave));
  
      stave->Print();
     }; // end loop over staves 

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
      TGeoVolume *LAYER = geom->MakeTube(fullname.c_str(), Al, layer_radius, layer_radius, layer_halfplainlength);
      LAYER->SetLineColor(kBlue);
      LAYER->SetTransparency(90);
      fullname="Layer"; //  + std::to_string(i);
      assembly_layer->AddNode(LAYER, i+1, new TGeoTranslation(0,0,0));

      
      pixel_barrel->AddNode(assembly_layer, i+1, new TGeoTranslation(0,0,0));
      pixelBarrelRings_Assembly->AddNode(ringLayer_Assembly,i+1,new TGeoTranslation(0,0,0));       
    }

   top->AddNode(pixel_barrel,1,new TGeoTranslation(0,0,0));
   top->AddNode(pixelBarrelRings_Assembly,1,new TGeoTranslation(0,0,0));

   return true;
} 



