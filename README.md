# VisualiserITK
A tool to visualuse ITK geometry for the ATLAS phase 2 upgrade. 
This package reads XML files with the ITK layouts and visuaize them using the EVE ROOT event display,
with additional output files (GDML, ROOT, JSON) inside the "out" directory.

# Installation instruction

To run this visulaization tool, you need several external packages:

## Get ROOT
Install ROOT 6.X and check that the variable $ROOTSYS is set. 

## Get xercesc

Define the directory $DIR where you want to install xercesc. Download it from
[http://ftp-stud.hs-esslingen.de/pub/Mirrors/ftp.apache.org/dist//xerces/c/3/sources/xerces-c-3.1.4.tar.gz](here). 
Then compile and install it:


```bash
tar -zvxf xerces-c-3.1.4.tar.gz
cd xercesc/3.1.4/
./configure --prefix=$DIR/xercesc/3.1.4/
make; make install
```

and create the "setup.sh" script (for the bash shell)  with these lines:

```bash
export XercesC_HOME=$DIR/xercesc/3.1.4
export PATH="$XercesC_HOME/bin:$PATH"
export LD_LIBRARY_PATH=$XercesC_HOME/lib:$LD_LIBRARY_PATH
source ./setup.sh
```

It is important to source this file before compiling the "VisualizeITK" package.

 
## Get VisualiserITK

Now get the package itself and compile it:


```bash
wget https://github.com/chekanov/VisualiserITK/archive/master.zip
unzip  master.zip
cd VisualiserITK-master
make
```
  
This creates the executable "main". Run it and you will see the EVE display with the layout.
In addition, the program creates several output files (ROOT, JSON, GDML) for further exploration of the layout.


# How o modify the input layout

This program reads the XML files from the directory "xml". The XML files are orginized in the subdirectoris. You can change 
the layout directory on the line "XMLReaderSvc reader("ATLAS-P2-ITK-16-00-00")" inside "main.cxx" and recompile it. 
The files are read from the directory "xml/layout" which have the following generic files: 
 
```bash
Materials.xml
PixelBarrel.xml
PixelModules.xml
PixelStave.xml
```

In fact, they are just the symbolic links to the corresponding ATLAS layout files. Simply run "source ./README" inside
the the "xml/layout" directory  to point to the necessary layout files. For ATLAS, you will need to download these XML files and make the corresponding symbolic links.
Typically, the XML files can be downloaded from the "share" directory of the [https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetDetDescr/InDetTrackingGeometryXML/trunk/share?order=name](InDetTrackingGeometryXML package).
 
# More explanation on codding

To visualize the layout, use the files such as ShowPixelBarrel.cxx,  ShowPixelEndcap.cxx etc.
The file names start from the string "Show" and have a single method:


```c++
process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoManager* geom)
```

Currently, only PixelBarrel is (almost) implemented. 


S.Chekanov (ANL)

 


