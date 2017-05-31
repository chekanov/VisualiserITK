///////////////////////////////////////////////////////////////////
// XMLReaderSvc.h, (c) ATLAS Detector software
// Tool to fill templates from XML files
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_XMLREADERSVC_H
#define INDETTRACKINGGEOMETRY_XMLREADERSVC_H

// STD
#include <fstream>
#include <vector>
#include<iostream>
// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
// Trk tools library
#include <XMLUtils.h>
#include <XMLGeoTemplates.h>

//#include "CLHEP/Geometry/defs.h"
//#include "CLHEP/Vector/ThreeVector.h"
//#include "CLHEP/Geometry/Transform3D.h"
#include <TROOT.h>
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
//#include "Math/XYZVector.h"
#include "Math/Vector4D.h"
#include "Math/Rotation3D.h"
#include "Math/RotationX.h"
#include "Math/RotationY.h"
#include "Math/RotationZ.h"
#include "Math/RotationZYX.h"
#include "Math/Boost.h"
#include "Math/BoostX.h"
#include "Math/BoostY.h"
#include "Math/BoostZ.h"
#include "Math/Transform3D.h"

using namespace xercesc;
using namespace std;



namespace Trk {
  class LayerMaterialProperties;
}

namespace InDet {

  class XMLReaderSvc : public XMLUtils {
  public:
    XMLReaderSvc(const std::string& name);
    virtual ~XMLReaderSvc();

    // AlgTool initialize and finalize methods
    bool initialize();
    bool finalize();

    // Function to overload to parse a node of an XML file
    virtual void parseNode(std::string section, DOMNode *node);

    ComponentTmp* getComponentTemplate(const std::string s) const;
    std::vector<StaveTmp*> getPixelStaveTemplate(unsigned int ilayer) const;
    BarrelLayerTmp *getPixelBarrelLayerTemplate(unsigned int ilayer) const;
    ModuleTmp* getModuleTemplate(const std::string s) const;
    ChipTmp* getChipTemplate(const std::string s) const;
    std::vector< BarrelLayerTmp *> getPixelBarrelLayers() const;

     // Stave building helpers
    double getHalfPlainLength(InDet::StaveTmp *staveTmp) const;
    double getZStep(InDet::StaveTmp *staveTmp) const;
    void computeZbounds(ROOT::Math::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &active_halflength) const;
    void computeRbounds(ROOT::Math::Transform3D trf, InDet::ModuleTmp* moduleTmp, double &rMin, double &rMax) const;
    double getStaveWidth(InDet::StaveTmp *staveTmp) const; 

    //const Trk::LayerMaterialProperties* getHomogeneousMaterial(std::string material_type);

  private:
    // XML templates parsing
    void parseMaterialXML(DOMNode* node);
    void parseComponentXML(DOMNode* node);
    void parseChipXML(DOMNode* node);
    void parseModuleXML(DOMNode* node);
    void parseStaveXML(DOMNode* node, std::vector< StaveTmp *>& vtmp);
    void parseBarrelLayerXML(DOMNode* node, std::vector< BarrelLayerTmp *>& vtmp);
    void parseEndcapXML(DOMNode* node, std::vector< EndcapLayerTmp *>& vtmp);

     void computeModuleSize(ModuleTmp *module);

    // Data members
    std::string m_dictionaryFileName;
    std::string m_xml_materials;
    std::string m_xml_pixmodules;
    std::string m_xml_pixStaves;
    std::string m_xml_pixBarrelLayers;
    std::string m_xml_pixEndcapLayers;
    std::string m_xml_sctmodules;
    std::string m_xml_sctStaves;
    std::string m_xml_sctBarrelLayers;
    std::string m_xml_sctEndcapLayers;

    std::vector< MaterialTmp *>    m_tmp_material;
    std::vector< ComponentTmp *>   m_tmp_component;
    std::vector< ChipTmp *>        m_tmp_chip;
    std::vector< ModuleTmp *>      m_tmp_module;
    std::vector< StaveTmp *>       m_tmp_pixStave;
    std::vector< BarrelLayerTmp *> m_tmp_pixBarrelLayer;
    std::vector< EndcapLayerTmp *> m_tmp_pixEndcapLayer;
    std::vector< StaveTmp *>       m_tmp_sctStave;
    std::vector< BarrelLayerTmp *> m_tmp_sctBarrelLayer;
    std::vector< EndcapLayerTmp *> m_tmp_sctEndcapLayer;

    bool m_doPix, m_doSCT, m_isGMX, m_createDict, m_readXMLfromDB;
  };
}

#endif //INDETTRACKINGGEOMETRY_XMLREADERSVC

