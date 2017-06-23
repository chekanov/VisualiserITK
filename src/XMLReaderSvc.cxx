#include "XMLReaderSvc.h"
#include "TMath.h"
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include "XMLGeoTemplates.h"

using namespace InDet;
using namespace std;


// name defines directory with the geometries
InDet::XMLReaderSvc::XMLReaderSvc(const std::string& name) {


  debug=true;
  m_layoutname=name;
  cout << "VisualiseXML: reading XML files from xml/"  << name << endl;
  string dir="xml/"+name+"/";
  m_xml_pixStaves=dir+"PixelStave.xml";
  m_xml_materials = dir+"Materials.xml";
  m_xml_pixmodules = dir+"PixelModules.xml";
  m_xml_pixBarrelLayers=dir+"PixelBarrel.xml";
  m_xml_pixEndcapLayers=dir+"PixelEndcap.xml";
  m_xml_SimpleServices=dir+"PixelSimpleService.xml";
  m_xml_EndcapDiskSupports=dir+"PixelEndcapDiskSupports.xml"; 


  cout << "  -> " << m_xml_materials << endl;
  cout << "  -> " <<  m_xml_pixStaves<< endl;
  cout << "  -> "  << m_xml_pixmodules << endl;
  cout << "  -> "  << m_xml_pixBarrelLayers << endl;
  cout << "  -> "  << m_xml_SimpleServices<< endl;
  cout << "  -> "  << m_xml_EndcapDiskSupports<< endl;
}


InDet::XMLReaderSvc::~XMLReaderSvc() {}


bool InDet::XMLReaderSvc::initialize()
{

  cout << "InDet::XMLReaderSvc::initialize()" << endl;

  if(!InitializeXML()) 
    cout << "Could not initialize XML helper." << endl;


   // WARNING: read components BEFORE materials
  ATH_MSG_INFO("Reading Material templates");
  parseFile(m_xml_materials.c_str(),"Materials","Component");
  parseFile(m_xml_materials.c_str(),"Materials","Material");
 // parseFile(m_xml_materials.c_str(),"PixelMaterials","MaterialWeight");
 // WARNING: read front-end chips BEFORE modules
  ATH_MSG_INFO("\n ======= Reading pixel FrontEndChip templates =======");
  parseFile(m_xml_pixmodules.c_str(),"PixelModules","FrontEndChip");
  ATH_MSG_INFO("\n ==== Reading pixel Module templates ======");
  parseFile(m_xml_pixmodules.c_str(),"PixelModules","Module");
  ATH_MSG_INFO("\n === Reading Pixel Stave templates =========");
  parseFile(m_xml_pixStaves.c_str(),"PixelStaves","PixelStave");
  ATH_MSG_INFO("Reading Pixel Barrel Layer templates");
  parseFile(m_xml_pixBarrelLayers.c_str(),"PixelBarrelLayers","PixelBarrelLayer");
  ATH_MSG_INFO("Reading Pixel Endcap Layer templates");
  parseFile(m_xml_pixEndcapLayers.c_str(),"PixelEndcapLayers","PixelEndcapRing");
  parseFile(m_xml_pixEndcapLayers.c_str(),"PixelEndcapLayers","PixelEndcapDisc");
  parseFile(m_xml_SimpleServices.c_str(),"PixelServices","SimpleService");
  parseFile(m_xml_EndcapDiskSupports.c_str(),"PixelRingSupports","PixelLayerSupportGeo");



  if(!TerminateXML()) 
    cout << "Could not terminate XML helper" << endl;

  
  return true;
}



bool InDet::XMLReaderSvc::finalize()
{
  return true;
}



InDet::ModuleTmp* InDet::XMLReaderSvc::getModuleTemplate(const std::string s) const
{
  // Get module template from list
  unsigned int n = m_tmp_module.size();

  for(unsigned int i=0;i<n;i++) {
    ModuleTmp *module = m_tmp_module.at(i);
    if(!module) continue;
    if(s.compare(module->name)) continue;
    return module;
  }
  cout << "InDet Module type '" << s << "' is not defined" << endl;
  return 0;
}


void InDet::XMLReaderSvc::parseNode(std::string section, DOMNode *node)
{
 
   if(section.find("Material")              != std::string::npos) parseMaterialXML(node);
  // else if(section.find("Component")        != std::string::npos) parseComponentXML(node);
  // else if(section.find("MaterialWeight")   != std::string::npos) parseMaterialXML(node);
   else if(section.find("FrontEndChip")     != std::string::npos) parseChipXML(node);
   else if(section.find("SimpleService")    != std::string::npos) parseSimpleServiceXML(node);
   else if(section.find("Module")           != std::string::npos) parseModuleXML(node);
   else if(section.find("PixelStave")       != std::string::npos) parseStaveXML(node,m_tmp_pixStave);
   else if(section.find("PixelBarrelLayer") != std::string::npos) parseBarrelLayerXML(node,m_tmp_pixBarrelLayer);
   else if(section.find("PixelEndcapRing" ) != std::string::npos) parseEndcapXML(node,m_tmp_pixEndcapLayer);
   else if(section.find("PixelEndcapDisc" ) != std::string::npos) parseEndcapXML(node,m_tmp_pixEndcapLayer);
   else if(section.find("PixelRingSupport") != std::string::npos) parseEndcapDiskSupportXML(node);
   else if(section.find("PixelLayerSupport") != std::string::npos) parseEndcapDiskSupportXML(node);
   else if(section.find("PixelRingSupportGeo") != std::string::npos) parseEndcapDiskSupportXML(node);
   else if(section.find("PixelLayerSupportGeo") != std::string::npos) parseEndcapDiskSupportXML(node);
//  else if(section.find("SCTStave")         != std::string::npos) parseStaveXML(node,m_tmp_sctStave);
//  else if(section.find("SCTBarrelLayer")   != std::string::npos) parseBarrelLayerXML(node,m_tmp_sctBarrelLayer);
//   else if(section.find("SCTEndcapRing")    != std::string::npos) parseEndcapXML(node,m_tmp_sctEndcapLayer);
//   else if(section.find("SCTEndcapDisc")    != std::string::npos) parseEndcapXML(node,m_tmp_sctEndcapLayer);
    return;
}

double InDet::XMLReaderSvc::getHalfPlainLength(InDet::StaveTmp *staveTmp) const
{
  if(staveTmp==0) return 0;

  // Number of modules
  int nmodplain = staveTmp->b_modn;
  int nmodtrans = 2*staveTmp->trans_pos.size();
  int NMOD = nmodtrans + nmodplain;

  // retrieve plain modules infos
  InDet::ModuleTmp* plainModuleTmp = getModuleTemplate(staveTmp->b_type);
  if(plainModuleTmp==0) return 0;
  double moduleplain_length = plainModuleTmp->length;
  double moduleplain_gap = staveTmp->b_gap;

  // the bins for transition modules are too long, it's normal
  double halfplainlength = 0.5*(NMOD*moduleplain_length + (NMOD-1)*moduleplain_gap);
  return halfplainlength;
}


// get the width of the stave
double InDet::XMLReaderSvc::getStaveWidth(InDet::StaveTmp *staveTmp) const
{
  if(staveTmp==0) return 0;

  // retrieve plain modules infos
  InDet::ModuleTmp* plainModuleTmp = getModuleTemplate(staveTmp->b_type);
  if(plainModuleTmp==0) return 0;
  double moduleplain_length = plainModuleTmp->length;
  double widthmax=plainModuleTmp->widthmax; 

  double width=widthmax;

  return width;
}




double InDet::XMLReaderSvc::getZStep(InDet::StaveTmp *staveTmp) const
{
  if(staveTmp==0) return 0;

  // retrieve plain modules infos
  InDet::ModuleTmp* plainModuleTmp = getModuleTemplate(staveTmp->b_type);
  if(plainModuleTmp==0) return 0;
  double moduleplain_length = plainModuleTmp->length;
  double moduleplain_gap = staveTmp->b_gap;

  // the bins for transition modules are too long, it's normal
  double zstep = moduleplain_length + moduleplain_gap;
  return zstep;
}


void InDet::XMLReaderSvc::computeRbounds(ROOT::Math::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &rMin, double &rMax) const
{

  std::vector<ROOT::Math::XYZVector >corners;

  // coordinates before any transform. 
  double xmod = moduleTmp->widthmax*.5;
  double ymod = moduleTmp->length*.5;
  double zmod = moduleTmp->thickness*.5;
  
  // top corners
  corners.push_back(ROOT::Math::XYZVector (-xmod, ymod, -zmod));
  corners.push_back(ROOT::Math::XYZVector ( xmod, ymod, -zmod));
  corners.push_back(ROOT::Math::XYZVector (-xmod, ymod,  zmod));
  corners.push_back(ROOT::Math::XYZVector ( xmod, ymod,  zmod));
  
  // bottom corners
  corners.push_back(ROOT::Math::XYZVector (-xmod, -ymod, -zmod));
  corners.push_back(ROOT::Math::XYZVector ( xmod, -ymod, -zmod));
  corners.push_back(ROOT::Math::XYZVector (-xmod, -ymod,  zmod));
  corners.push_back(ROOT::Math::XYZVector ( xmod, -ymod,  zmod));
  
  // faces
  corners.push_back(ROOT::Math::XYZVector ( 0.5*xmod, ymod,  zmod));
  corners.push_back(ROOT::Math::XYZVector ( 0.5*xmod, ymod, -zmod));
  corners.push_back(ROOT::Math::XYZVector (-0.5*xmod, ymod,  zmod));
  corners.push_back(ROOT::Math::XYZVector (-0.5*xmod, ymod, -zmod));
  
  corners.push_back(ROOT::Math::XYZVector ( 0.5*xmod, -ymod,  zmod));
  corners.push_back(ROOT::Math::XYZVector ( 0.5*xmod, -ymod, -zmod));
  corners.push_back(ROOT::Math::XYZVector (-0.5*xmod, -ymod,  zmod));
  corners.push_back(ROOT::Math::XYZVector (-0.5*xmod, -ymod, -zmod));
  double vMin=9999.;
  double vMax=-9999.;
  
  for(unsigned int i=0; i<corners.size(); i++)
    {
      ROOT::Math::XYZVector p = trf*corners[i];
      double r = sqrt(p.x()*p.x()+p.y()*p.y());

      vMin = std::min(vMin,r);
      vMax = std::max(vMax,r);
    }

  rMin = std::min(rMin,vMin);
  rMax = std::max(rMax,vMax);

}


void InDet::XMLReaderSvc::computeZbounds(ROOT::Math::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &active_halflength) const
{

  std::vector<ROOT::Math::XYZVector >corners;

  // coordinates before any transform. 
  double xmod = moduleTmp->widthmax*.5;
  double ymod = moduleTmp->length*.5;
  double zmod = moduleTmp->thickness*.5;

  // top corners
  corners.push_back( ROOT::Math::XYZVector (-xmod, ymod, -zmod));
  corners.push_back( ROOT::Math::XYZVector ( xmod, ymod, -zmod));
  corners.push_back( ROOT::Math::XYZVector (-xmod, ymod,  zmod));
  corners.push_back( ROOT::Math::XYZVector ( xmod, ymod,  zmod));

  // bottom corners
  corners.push_back( ROOT::Math::XYZVector (-xmod, -ymod, -zmod));
  corners.push_back( ROOT::Math::XYZVector ( xmod, -ymod, -zmod));
  corners.push_back( ROOT::Math::XYZVector (-xmod, -ymod,  zmod));
  corners.push_back( ROOT::Math::XYZVector ( xmod, -ymod,  zmod));

  double vMin=9999.;
  double vMax=-9999.;

  for(unsigned int i=0; i<corners.size(); i++)
    {
      ROOT::Math::XYZVector p = trf*corners[i];
      double z = p.z();
      vMin = std::min(vMin,z);
      vMax = std::max(vMax,z);
    }

  active_halflength = std::max(std::fabs(vMin),active_halflength);
  active_halflength = std::max(std::fabs(vMax),active_halflength);

}



void InDet::XMLReaderSvc::parseBarrelLayerXML(DOMNode* node, std::vector< InDet::BarrelLayerTmp *>& vtmp)
{

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_layername      = transcode("Name");
  XMLCh* TAG_layerindex     = transcode("Number");
  XMLCh* TAG_layerradius    = transcode("LayerRadius");
  XMLCh* TAG_nstaves        = transcode("NStaves");
  XMLCh* TAG_stavetype      = transcode("StaveType");
  XMLCh* TAG_stavetilt      = transcode("StaveTilt");
  XMLCh* TAG_stavezoffset   = transcode("StaveZOffset");
  XMLCh* TAG_phioffset      = transcode("LayerPhiOfModuleZero");

  BarrelLayerTmp *layer = new BarrelLayerTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant layer parameters
    if (XMLString::equals(currentElement->getTagName(),TAG_layername))         layer->name          = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_layerindex))   layer->index         = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_layerradius))  layer->radius        = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_nstaves))      layer->stave_n       = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stavetype))    layer->stave_type    = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_stavetilt))    layer->stave_tilt    = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stavezoffset)) layer->stave_zoffset = atof(getString(currentNode))/2.0;
    else if (XMLString::equals(currentElement->getTagName(),TAG_phioffset))    layer->phioffset     = atof(getString(currentNode));
  } // End of loop on layer node elements

 // Register layer template 
  if((layer->name).size()>0 && layer->index>=0) {
    unsigned int min_size = layer->index+1;
    if(vtmp.size()<min_size) vtmp.resize(min_size,0);
    vtmp.at(layer->index) = layer;
    if(debug)
      layer->Print();
  }
}



void InDet::XMLReaderSvc::parseChipXML(DOMNode* node)
{
  ATH_MSG_DEBUG("\n =========== Parse chips XML info =============");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name      = transcode("name");
  XMLCh* TAG_length    = transcode("chipLength");
  XMLCh* TAG_width     = transcode("chipWidth");
  XMLCh* TAG_pitch_eta = transcode("pitchEta");
  XMLCh* TAG_pitch_phi = transcode("pitchPhi");
  XMLCh* TAG_rows      = transcode("rows");
  XMLCh* TAG_columns   = transcode("columns");
  XMLCh* TAG_edges     = transcode("Edges");
  XMLCh* TAG_ewide     = transcode("wide");
  XMLCh* TAG_enarrow   = transcode("narrow");
  XMLCh* TAG_elength   = transcode("inlength");

  ChipTmp *chip = new ChipTmp;
  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant chip parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_name))           chip->name     = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_length))    chip->length   = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_width))     chip->width    = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_pitch_eta)) chip->pitchEta = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_pitch_phi)) chip->pitchPhi = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_rows))      chip->rows     = atoi(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_columns))   chip->columns  = atoi(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_edges)){
      DOMNodeList* edgesList = currentElement->getChildNodes();
      const XMLSize_t edgesCount = edgesList->getLength();
      for( XMLSize_t xx = 0; xx < edgesCount; ++xx ) {
        DOMNode* edgeNode = edgesList->item(xx);
        if(edgeNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element
        // Found node which is an Element. Re-cast node as element
        DOMElement* edgeElement = dynamic_cast< xercesc::DOMElement* >( edgeNode );
        if( XMLString::equals(edgeElement->getTagName(), TAG_ewide))        chip->edgew   = atof(getString(edgeNode));
        else if( XMLString::equals(edgeElement->getTagName(), TAG_enarrow)) chip->edgen   = atof(getString(edgeNode));
        else if( XMLString::equals(edgeElement->getTagName(), TAG_elength)) chip->edgel   = atof(getString(edgeNode));
      }// End of loop on edges node elements
    }
  } // End of loop on chip node elements

  // Register chip template
  if((chip->name).size()>0) {
    m_tmp_chip.push_back(chip);
    if(debug)
      chip->Print();
  }
}

//parsing simple service info added by (af)
void InDet::XMLReaderSvc::parseSimpleServiceXML(DOMNode* node)
{
  
  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name = transcode("NAME");
  XMLCh* TAG_rmin = transcode("RMIN");
  XMLCh* TAG_rmax = transcode("RMAX");
  XMLCh* TAG_zmin = transcode("ZMIN");
  XMLCh* TAG_zmax = transcode("ZMAX");
   
  SimpleServiceTubeTmp *simpleService = new SimpleServiceTubeTmp;
  	for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
        	DOMNode* currentNode = list->item(xx);
                if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element
  
                                         // Found node which is an Element. Re-cast node as element
          	 DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
	
	if( XMLString::equals(currentElement->getTagName(), TAG_name))      simpleService->Name = getString(currentNode);
   	else if( XMLString::equals(currentElement->getTagName(), TAG_rmin)) simpleService->Rmin = atof(getString(currentNode));
   	else if( XMLString::equals(currentElement->getTagName(), TAG_rmax)) simpleService->Rmax = atof(getString(currentNode));
 	else if( XMLString::equals(currentElement->getTagName(), TAG_zmin)) simpleService->Zmin = atof(getString(currentNode));
    	else if( XMLString::equals(currentElement->getTagName(), TAG_zmax)) simpleService->Zmax = atof(getString(currentNode));
        
        } //end of loop on node elements
 	       
        cout<<"debug check for simple service"<<endl;                 
         if((simpleService->Name)!=0) {
      		 m_tmp_SimpleService.push_back(simpleService);
      
          simpleService->Print();
	  cout<<"parsed Simple Service info"<<endl;
	  
       	  }
        }


//parse Endcap Disk Supports --------------------------------------------------------------------------------------------------------(af)        
void InDet::XMLReaderSvc::parseEndcapDiskSupportXML(DOMNode* node)
{

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();
  XMLCh* TAG_name      = transcode("name");
  XMLCh* TAG_rmin    = transcode("rmin");
  XMLCh* TAG_rmax     = transcode("rmax");
  XMLCh* TAG_thickness = transcode("thickness");
  XMLCh* TAG_ringSupportGeo = transcode("RingSupportGeo");
  XMLCh* TAG_layerSupportGeo = transcode("LayerSupportGeo");
  XMLCh* TAG_r = transcode("r");
  XMLCh* TAG_z = transcode("z");
  XMLCh* TAG_layer = transcode("Layer");



   
  SimpleServiceTubeTmp *endcapLayerSupport = new SimpleServiceTubeTmp;
        for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
                DOMNode* currentNode = list->item(xx);
                if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; //not an element
  
  DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

        if( XMLString::equals(currentElement->getTagName(), TAG_name))      endcapLayerSupport->Name = getString(currentNode);
        else if( XMLString::equals(currentElement->getTagName(), TAG_r)){
		 endcapLayerSupport->Rmin =getVectorDouble(currentNode)[0];
		 endcapLayerSupport->Rmax =getVectorDouble(currentNode)[1];
        }
	 //else if( XMLString::equals(currentElement->getTagName(), TAG_rmax)) endcapLayerSupport->Rmax = atof(getString(currentNode));
        else if( XMLString::equals(currentElement->getTagName(), TAG_z)){
		 endcapLayerSupport->Zmin = getVectorDouble(currentNode)[0];
		 endcapLayerSupport->Zmax = getVectorDouble(currentNode)[1];
		 }
        //else if( XMLString::equals(currentElement->getTagName(), TAG_zmax)) endcapLayerSupport->Zmax = atof(getString(currentNode));
	} //end loop on node elements

	if((endcapLayerSupport->Name)!=0) {
                 m_tmp_SimpleService.push_back(endcapLayerSupport);

          endcapLayerSupport->Print();
          cout<<"parsed endcap support info"<<endl;
   }
}



void InDet::XMLReaderSvc::parseEndcapXML(DOMNode* node, std::vector< InDet::EndcapLayerTmp *>& vtmp)
{


  ATH_MSG_DEBUG("\n =========== Parse Endcap  XML info =============");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name            = transcode("Name");
  XMLCh* TAG_layernum        = transcode("LayerNumber");
  XMLCh* TAG_layerpos        = transcode("LayerPosition");
  XMLCh* TAG_discnum         = transcode("DiscNumber");
  XMLCh* TAG_radius          = transcode("Radius");
  XMLCh* TAG_radiustype      = transcode("RadiusType");
  XMLCh* TAG_nsectors        = transcode("NumberOfSectors");
  XMLCh* TAG_modtype         = transcode("RingModuleType");
  XMLCh* TAG_zoffset         = transcode("RingModuleZSeparation");
  XMLCh* TAG_phioffset       = transcode("RingPhiOfModuleZero");
  XMLCh* TAG_material        = transcode("RingSupportMaterial");
  XMLCh* TAG_ringpos         = transcode("RingPositions");
  XMLCh* TAG_stereoInner     = transcode("RingModuleStereoAngleInner");
  XMLCh* TAG_stereoOuter     = transcode("RingModuleStereoAngleOuter");
  XMLCh* TAG_stereoSep       = transcode("RingModuleStereoSeparation");
  XMLCh* TAG_doublesided     = transcode("DoubleSided");
  XMLCh* TAG_useDiscSurface  = transcode("UseDiscSurface");
  XMLCh* TAG_SplitOffset  = transcode("SplitOffset");

// temporary variables
  std::string name;
  int layer_index = -1;
  int layer_pos   = -1;
  int disc_index  = -1;
  std::string material = "DefaultPixelRingMaterial";
  std::string rtype;
  double stereoInner = 0.;
  double stereoOuter = 0.;
  double stereoSep = 0.;
  bool double_sided = false;
  bool usediscsurf  = false;
  double splitOffSet = 0;

  std::vector<std::string> tmpmodtype;
  std::vector<double> tmpradius;
  std::vector<double> tmpringpos;
  std::vector<int>    tmpnsectors;
  std::vector<double> tmpzoffset;
  std::vector<double> tmpphioffset;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {

    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
 // Look for relevant layer parameters
    if (XMLString::equals(currentElement->getTagName(),TAG_name))                 name          = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_layernum))        layer_index   = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_layerpos))        layer_pos     = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_discnum))         disc_index    = atoi(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_material))        material      = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_radiustype))      rtype         = getString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_stereoInner))     stereoInner   = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stereoOuter))     stereoOuter   = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_stereoSep))       stereoSep     = atof(getString(currentNode));
    else if (XMLString::equals(currentElement->getTagName(),TAG_doublesided))     double_sided  = getBoolean(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_useDiscSurface))  usediscsurf   = getBoolean(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_modtype))         tmpmodtype    = getVectorString(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_radius))          tmpradius     = getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_nsectors))        tmpnsectors   = getVectorInt(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_zoffset))         tmpzoffset    = getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_phioffset))       tmpphioffset  = getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_ringpos))         tmpringpos    = getVectorDouble(currentNode);
    else if (XMLString::equals(currentElement->getTagName(),TAG_SplitOffset))     splitOffSet   = atoi(getString(currentNode));

  }

  // If different number of entries for rings fields, use the values of ring 0 everywhere
  unsigned int nrings = tmpringpos.size();
  if(tmpmodtype.size() != nrings && tmpmodtype.size()>0 ){
    std::string modtype = tmpmodtype.at(0);
    tmpmodtype.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpmodtype.push_back(modtype);
  }else if(tmpmodtype.size() == 0){
    ATH_MSG_WARNING("No module type defined for rings --> No template built");
    return;
  }

 if(tmpzoffset.size()>0){
    if(tmpzoffset.size() != nrings){
      double zoffs = tmpzoffset.at(0);
      tmpzoffset.clear();
      for (unsigned int ir = 0 ; ir < nrings; ir++) tmpzoffset.push_back(zoffs);
    }
  }else{
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpzoffset.push_back(0.);
  }

  if(tmpphioffset.size()>0){
    if(tmpphioffset.size() != nrings){
      double phioffs = tmpphioffset.at(0);
      tmpphioffset.clear();
      for (unsigned int ir = 0 ; ir < nrings; ir++) tmpphioffset.push_back(phioffs);
    }
  }else{
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpphioffset.push_back(0.);
  }

  if(tmpnsectors.size() != nrings && tmpnsectors.size()>0){
    double nsec = tmpnsectors.at(0);
    tmpnsectors.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpnsectors.push_back(nsec);
  }else if (tmpnsectors.size() == 0){
    ATH_MSG_WARNING("Number of sectors not defined for rings --> No template built");
      return;
  }

  if(tmpradius.size() != nrings && tmpradius.size()>0){
    double rad = tmpradius.at(0);
    tmpradius.clear();
    for (unsigned int ir = 0 ; ir < nrings; ir++) tmpradius.push_back(rad);
  }else if (tmpradius.size() == 0 ){
    ATH_MSG_WARNING("Rings radii not defined--> No template built");
    return;
  }

  EndcapLayerTmp *layer = new EndcapLayerTmp;

  layer->name                = name;
  layer->support_material    = material;
  layer->double_sided        = double_sided;
  layer->stereoI             = stereoInner;
  layer->stereoO             = stereoOuter;
  layer->stereoSep           = stereoSep;
  layer->useDiscSurface      = usediscsurf;
  layer->splitOffSet         = splitOffSet;

  // disc layer
  if(layer_index<0) {
    layer->ilayer = disc_index;
    layer->layer_pos = disc_index;
    layer->isDisc = true;
  // ring layer
  } else {
    layer->ilayer = layer_index;
    layer->isDisc = false;
    if(layer_pos<0) layer->layer_pos = layer_index;
    else layer->layer_pos = layer_pos;
  }

  // loop over rings to store the info in template
  for(unsigned int ir=0;ir<tmpringpos.size();ir++){

    // retrieve module info to compute inner/outer rings radii 
    ModuleTmp* moduleTmp = getModuleTemplate(tmpmodtype.at(ir));

    if(moduleTmp == 0) {
      cout << "Module type " <<  tmpmodtype.at(ir)<< " is not defined --> Skipping ring template " << ir << " Layer " << layer_index << endl; return;
    }

    double modlength = moduleTmp->length;
    double modthick  = moduleTmp->thickness;

    layer->modtype.push_back(tmpmodtype.at(ir));
    layer->ringpos.push_back(tmpringpos.at(ir));
    layer->zoffset.push_back(tmpzoffset.at(ir));
    layer->phioffset.push_back(tmpphioffset.at(ir));
    layer->nsectors.push_back(tmpnsectors.at(ir));

    // compute ring radii
    double innerRadius = tmpradius.at(ir);
    double outerRadius = tmpradius.at(ir);
    if (rtype.compare("Inner")==0) outerRadius += modlength;
    else innerRadius -= modlength;
    layer->innerRadius.push_back(innerRadius);
    layer->outerRadius.push_back(outerRadius);
    // compute ring thickness (module thickness + zoffset (can be negative))
    double tmpthick  = modthick + std::fabs(tmpzoffset.at(ir));
    if(double_sided) tmpthick += modthick+2*stereoSep;
    layer->thickness.push_back(tmpthick);
  }

 // Register endcap layer template
  vtmp.push_back(layer);
  if(debug)
    layer->Print();

  return;

}


void InDet::XMLReaderSvc::parseModuleXML(DOMNode* node)
{
  ATH_MSG_DEBUG("\n ============= Parse modules XML info ================");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name      = transcode("moduleName");
  XMLCh* TAG_chip      = transcode("chipName");
  XMLCh* TAG_length    = transcode("lengthInChips");
  XMLCh* TAG_widthmax  = transcode("widthMaxInChips");
  XMLCh* TAG_widthmin  = transcode("widthMinInChips");
  XMLCh* TAG_sensthickness = transcode("sensorThickness");
  XMLCh* TAG_chipthickness = transcode("chipThickness");
  XMLCh* TAG_hybdthickness = transcode("hybridThickness");

  ModuleTmp *module = new ModuleTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant module parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_name))            module->name           = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_chip))       module->chip_type      = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_length))     module->lengthChips    = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_widthmax))   module->widthMaxChips  = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_widthmin))   module->widthMinChips  = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_sensthickness)){
      module->thickness_sensor  = atof(getString(currentNode));
      module->thickness  += atof(getString(currentNode));
    }
    else if( XMLString::equals(currentElement->getTagName(), TAG_chipthickness))  module->thickness  += atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_hybdthickness))  module->thickness  += atof(getString(currentNode));
  } // End of loop on module node elements

  if(module->chip_type.size()==0) return;
  ChipTmp* chip = getChipTemplate(module->chip_type);
  module->pitchEta = chip->pitchEta;
  module->pitchPhi = chip->pitchPhi;

  // Register module template
  if((module->name).size()>0) {
    computeModuleSize(module);
    m_tmp_module.push_back(module);
    if(debug)
      module->Print();
  }

}

InDet::ChipTmp* InDet::XMLReaderSvc::getChipTemplate(const std::string s) const
{
  // Get chip template from list
  unsigned int n = m_tmp_chip.size();
  for(unsigned int i=0;i<n;i++) {
    ChipTmp *chip = m_tmp_chip.at(i);
    if(!chip) continue;
    if(s.compare(chip->name)) continue;
    return chip;
  }

  cout << "InDet Chip type '" << s << "' is not defined"<<endl;
  return 0;
}


void InDet::XMLReaderSvc::parseMaterialXML(DOMNode* node)
{


  ATH_MSG_DEBUG("\n ============ Parse material XML info ==================\n");

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_type        = transcode("MaterialType");
  XMLCh* TAG_thickness   = transcode("Thickness");
  XMLCh* TAG_component   = transcode("Component");
  XMLCh* TAG_cfrac       = transcode("Fraction");
  XMLCh* TAG_cname       = transcode("Name");


   MaterialTmp *material = new MaterialTmp();

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

   // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant module parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_type))              material->name  = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_thickness))    material->thickness = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_component)) {
      DOMNodeList* cmpList = currentElement->getChildNodes();
      const XMLSize_t cmpCount = cmpList->getLength();
      for( XMLSize_t xx = 0; xx < cmpCount; ++xx ) {
        DOMNode* cmpNode = cmpList->item(xx);
        if(cmpNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element
        // Found node which is an Element. Re-cast node as element
        DOMElement* cmpElement = dynamic_cast< xercesc::DOMElement* >( cmpNode );
        if( XMLString::equals(cmpElement->getTagName(),TAG_cname))      material->components.push_back(getString(cmpNode));
        else if( XMLString::equals(cmpElement->getTagName(),TAG_cfrac)) material->fractions.push_back(atof(getString(cmpNode)));
      }
    } // end component loop
  } // end material loop

 if((material->name).size()>0) {

    // compute X0, L0, A, Z and density of material based on components properties
    int ncomponents = material->components.size();
    double X0inv = 0.;
    double L0inv = 0.;
    double A = 0.;
    double Z = 0.;
    double rho = 0.;

    if(ncomponents==0) {
      cout << "Material " << material->name << " contains 0 components --> Skip" << endl;
    }else{
      for(int ic =0; ic < ncomponents;ic++){
        ComponentTmp* comp = getComponentTemplate(material->components.at(ic));
        if(comp==0) continue;
        double fraction = material->fractions.at(ic);
        X0inv += fraction/comp->X0;
        L0inv += fraction/comp->L0;
        A += fraction*comp->A;
        Z += fraction*comp->Z;
        rho += fraction*comp->density;
        // note: approximation for X0 computation when data is missing
        // X0 = 716.4/[Z(Z+1).ln(287/sqrt(Z))] g.cm-3
        // Then divide by density to get X0 in cm

      }

      material->X0 = 1./X0inv;
      material->L0 = 1./L0inv;
      material->A = A;
      material->Z = Z;
      material->density = rho;

      // Register material template
      m_tmp_material.push_back(material);
      if (debug) material->Print();
    }
  }

}


void InDet::XMLReaderSvc::computeModuleSize(InDet::ModuleTmp *module)
{

  if(module == 0) return;
  if(module->chip_type.size()==0) return;

  ChipTmp* chip = getChipTemplate(module->chip_type);
  module->length = module->lengthChips*chip->length+2*chip->edgel;
  if(module->widthMinChips==0) module->widthMinChips = module->widthMaxChips;
  module->widthmin  = module->widthMinChips*chip->width+chip->edgew;
  module->widthmax  = module->widthMaxChips*chip->width+chip->edgew;

  if(chip->name.find("RD53") != std::string::npos) {
    module->length = module->lengthChips*chip->length+(module->lengthChips-1)*2.*chip->edgel;
    module->widthmax = module->widthMaxChips*chip->width+(module->widthMaxChips-1)*2.*chip->edgew;
    module->widthmin = module->widthmax;
  }
  else {
    if(module->chip_type=="SingleChip"){
      module->widthmin += chip->edgen;
      module->widthmax += chip->edgen;
    }else{
      module->widthmin += chip->edgew;
      module->widthmax += chip->edgew;
    }
  }
}


InDet::ComponentTmp* InDet::XMLReaderSvc::getComponentTemplate(const std::string s) const
{
  // Get material template from list
  unsigned int n = m_tmp_component.size();

  for(unsigned int i=0;i<n;i++) {
    ComponentTmp *component = m_tmp_component.at(i);
    if(!component) continue;
    if(s.compare(component->name)) continue;
    return component;
  }
  // ("InDet Component type '" << s << "' is not defined");
  return 0;
}


std::vector<InDet::StaveTmp*> InDet::XMLReaderSvc::getPixelStaveTemplate(unsigned int ilayer) const
{

  std::vector<InDet::StaveTmp*> staves;

  // Get staves from template list
  unsigned int n = m_tmp_pixStave.size();
  for(unsigned int istave=0;istave<n;istave++){
    StaveTmp *stave = m_tmp_pixStave.at(istave);
    if(stave->layer == int(ilayer)) staves.push_back(stave);
  }

  if(staves.size()==0) cout << "InDet no Pixel Stave template found for layer " << ilayer << endl;

  return staves;
}


std::vector< BarrelLayerTmp *> InDet::XMLReaderSvc::getPixelBarrelLayers() const
{

  std::vector< BarrelLayerTmp *> layers;
  // Get staves from template list
  unsigned int n = m_tmp_pixBarrelLayer.size();
  for(unsigned int is=0;is<n;is++){
    BarrelLayerTmp *layer = m_tmp_pixBarrelLayer.at(is);
    layers.push_back(layer);
  }
  return layers;
}


std::vector< EndcapLayerTmp *> InDet::XMLReaderSvc::getPixelEndcapLayers() const
{

  std::vector< EndcapLayerTmp *> layers;
  
    unsigned int n = m_tmp_pixEndcapLayer.size();	
     for(unsigned int is=0;is<n;is++){
          EndcapLayerTmp *layer = m_tmp_pixEndcapLayer.at(is);
              layers.push_back(layer);
     }
      return layers;
  }

// get material templates (af)
std::vector< MaterialTmp *> InDet::XMLReaderSvc::getMaterials() const
{

  std::vector< MaterialTmp *> materials;
unsigned int n = m_tmp_material.size();
     for(unsigned int is=0;is<n;is++){
          MaterialTmp *material = m_tmp_material.at(is);
             materials.push_back(material);
     }
      return materials;
  }

InDet::BarrelLayerTmp* InDet::XMLReaderSvc::getPixelBarrelLayerTemplate(unsigned int ilayer) const
{
  // Get layer index from list
  unsigned int n = m_tmp_pixBarrelLayer.size();

  if(ilayer>n-1) {
    cout << "InDet Pixel barrel template for layer " << ilayer << " is not defined" << endl;
    return 0;
  }

  BarrelLayerTmp *layer = m_tmp_pixBarrelLayer.at(ilayer);
  return layer;
}

std::vector<SimpleServiceTubeTmp *> InDet::XMLReaderSvc::getPixelSimpleServiceTubeTemplates() const
{
 // Get material template from list
   unsigned int n = m_tmp_SimpleService.size();
   std::vector< SimpleServiceTubeTmp *> simpleServices;

     for(unsigned int i=0;i<n;i++) {
        SimpleServiceTubeTmp *SimpleServiceTube = m_tmp_SimpleService.at(i);
                     simpleServices.push_back( SimpleServiceTube);
     }
	return simpleServices;

                    }


void InDet::XMLReaderSvc::parseComponentXML(DOMNode* node)
{
  cout << "Parse component XML info" << endl;

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name        = transcode("Name");
  XMLCh* TAG_x0          = transcode("X0");
  XMLCh* TAG_l0          = transcode("L0");
  XMLCh* TAG_A           = transcode("A");
  XMLCh* TAG_Z           = transcode("Z");
  XMLCh* TAG_density     = transcode("Density");

}



void InDet::XMLReaderSvc::parseStaveXML(DOMNode* node, std::vector< InDet::StaveTmp *>& vtmp)
{

  DOMNodeList* list = node->getChildNodes();
  const XMLSize_t nodeCount = list->getLength();

  // Create XML 16 bit strings
  XMLCh* TAG_name        = transcode("Name");
  XMLCh* TAG_type        = transcode("Type");
  XMLCh* TAG_layer       = transcode("Layer");
  XMLCh* TAG_b_modn      = transcode("NBarrelModules");
  XMLCh* TAG_b_type      = transcode("BarrelModuleType");
  XMLCh* TAG_b_gap       = transcode("BarrelModuleGap");
  XMLCh* TAG_b_tilt      = transcode("BarrelModuleTilt");
  XMLCh* TAG_b_angle     = transcode("BarrelModuleAngle");
  XMLCh* TAG_b_sameAngle = transcode("BarrelModuleSameAngle");
  XMLCh* TAG_b_stereoI   = transcode("BarrelModuleStereoAngleInner");
  XMLCh* TAG_b_stereoO   = transcode("BarrelModuleStereoAngleOuter");
  XMLCh* TAG_b_stereoSep = transcode("BarrelModuleStereoSeparation");
  XMLCh* TAG_b_rshift    = transcode("BarrelModuleRShift");
  XMLCh* TAG_trans_tilt  = transcode("TransitionModuleTilt");
  XMLCh* TAG_trans_angle = transcode("TransitionModuleAngle");
  XMLCh* TAG_trans_type  = transcode("TransitionModuleType");
  XMLCh* TAG_trans_pos   = transcode("TransitionPosOnStavePlane");
  XMLCh* TAG_trans_gap   = transcode("TransitionGap");
  XMLCh* TAG_alp_pos     = transcode("MountainPosOnStavePlane");
  XMLCh* TAG_alp_tilt    = transcode("MountainModuleTilt");
  XMLCh* TAG_alp_angle   = transcode("MountainModuleAngle");
  XMLCh* TAG_alp_type    = transcode("MountainModuleType");
  XMLCh* TAG_alp_rshift  = transcode("MountainModuleRShift");
  XMLCh* TAG_material    = transcode("StaveMaterial");
  XMLCh* TAG_length      = transcode("StaveSupportLength");
  XMLCh* TAG_doublesided = transcode("DoubleSided");

 StaveTmp *stave = new StaveTmp;

  for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
    DOMNode* currentNode = list->item(xx);
    if(currentNode->getNodeType() != DOMNode::ELEMENT_NODE) continue; // not an element

    // Found node which is an Element. Re-cast node as element
    DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );

    // Look for relevant stave parameters
    if( XMLString::equals(currentElement->getTagName(), TAG_name))             stave->name               = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_type))        stave->type               = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_layer))       stave->layer              = atoi(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_modn))      stave->b_modn             = int(2*atof(getString(currentNode)));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_type))      stave->b_type             = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_gap))       stave->b_gap              = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_tilt))      stave->b_tilt             = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_angle))     stave->b_angle            = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_sameAngle)) stave->b_sameAngle        = getBoolean(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_stereoI))   stave->b_stereoI          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_stereoO))   stave->b_stereoO          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_stereoSep)) stave->b_stereoSep        = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_b_rshift))    stave->b_rshift           = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_tilt))  stave->trans_tilt         = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_angle)) stave->trans_angle        = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_type))  stave->trans_type         = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_pos))   stave->trans_pos          = getVectorDouble(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_trans_gap))   stave->trans_gap          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_tilt))    stave->alp_tilt           = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_angle))   stave->alp_angle          = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_type))    stave->alp_type           = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_pos))     stave->alp_pos            = getVectorDouble(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_alp_rshift))  stave->alp_rshift         = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_length))      stave->support_halflength = atof(getString(currentNode));
    else if( XMLString::equals(currentElement->getTagName(), TAG_material))    stave->support_material   = getString(currentNode);
    else if( XMLString::equals(currentElement->getTagName(), TAG_doublesided)) stave->double_sided       = getBoolean(currentNode);
  } // End of loop on stave node elements


  // Register stave template
  if((stave->name).size()>0 && stave->layer>=0) {
    vtmp.push_back(stave);
    //if(msgLvl(MSG::DEBUG))
     stave->Print();
  }


}





