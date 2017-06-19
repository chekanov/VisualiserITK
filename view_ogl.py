# a simple OGL viewer of geometry
# S.Chekanov (ANL)

import sys
from ROOT import *
import ROOT
from array import array
from math import *

myinput="interactive"
NN=0
if (len(sys.argv) ==2):
   myinput = sys.argv[1]



xname="Inclined_Duals.gdml"
TGeoManager.Import("out/"+xname);
gGeoManager.SetMaxVisNodes(100000);
gGeoManager.ViewLeaves(True)
gGeoManager.SetVisLevel(64) # change it to make faster 
allvolumes = gGeoManager.GetListOfVolumes();
for i in range(allvolumes.GetEntries()):
            vol= allvolumes.At(i);
            vol.VisibleDaughters(True);
            name = vol.GetName();
            print name
	    print vol.IsVisible()
            vol.SetTransparency(50)
            vol.SetVisibility(True)
	    print vol.IsVisible()
	    
            if (name.find("PST")>-1):
                       vol.SetLineColor(ROOT.kGreen)
                       vol.SetVisibility(True)
                       vol.SetTransparency(50)

            if (name.find("DoubleLength_RD53_20x19_thin_2")>-1):
                       vol.SetLineColor(ROOT.kRed)
                       vol.SetVisibility(True)
                       vol.SetTransparency(50)

            if (name.find("FourChip_RD53_2")>-1):
                       vol.SetLineColor(ROOT.kRed)
                       vol.SetVisibility(True)
                       vol.SetTransparency(50)


#gEve.AddGlobalElement(axis);
#topvolume.Draw("ogl")

#kDis=0.1
#gGeoManager.CheckOverlaps(kDis);
#gGeoManager.PrintOverlaps();

gGeoManager.SetClipping(1)

gGeoManager.GetTopVolume().Draw("ogl")

"""
TEveManager.Create();
tn =TEveGeoTopNode(gGeoManager, gGeoManager.GetTopNode());
tn.SetVisLevel(4);
gEve.AddGlobalElement(tn);
gEve.FullRedraw3D(kTRUE);

# EClipType not exported to CINT (see TGLUtil.h):
# 0 - no clip, 1 - clip plane, 2 - clip box
v = gEve.GetDefaultGLViewer()
v.GetClipSet().SetClipType( TGLClip.EType(1) ) 
v.ColorSet().Background().SetColor(ROOT.kWhite);
v.CurrentCamera().RotateRad(-1.57, 0.0);

v.DoDraw();

"""

if (myinput != "-b"):
              if (raw_input("Press any key to exit") != "-9999"):
                          sys.exit(1);
