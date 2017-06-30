# VisualiserITK
A tool to visualuse ITK geometry for the ATLAS phase 2 upgrade. 
This package reads XML files with the ITK layouts and visuaize them using the EVE ROOT event display,
with additional output files (GDML, ROOT, JSON, TXT) inside the "out" directory.

# Installation instruction

To run this visulaization tool, you need several external packages:

## Get ROOT
Install ROOT 6.X and check that the variable $ROOTSYS is set. 

## Get xercesc

Define the directory $DIR where you want to install xercesc. Download it from
[here](http://ftp-stud.hs-esslingen.de/pub/Mirrors/ftp.apache.org/dist//xerces/c/3/sources/xerces-c-3.1.4.tar.gz). 
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
  
This creates the executable "A_RUN". Run it and you will see the EVE display with the layout. In order to go through all the available layouts close eve and allow the program to run through all "styles" and "layouts".
In addition, the program creates several output files (ROOT, JSON, GDML, TXT) for further exploration of the layout.
At the end of each output file is a number, ie. "Inclined_Quads_3.root", which corresponds to a style or complexity of the visualisation.

## Styles

0: full detector.
1: no modules shown, only support structures.
2: removes structures that only exist in the -z region of the detector (displays half the detector).
3: only the inner 2 layers of the detector (US build).

# How to modify the input layout

This program reads the XML files from the directory "xml". The XML files are orginized into subdirectories. You can change 
the layout directory on the line "XMLReaderSvc reader("ATLAS-P2-ITK-16-00-00")" inside "main.cxx" and recompile it. 
The files are read from the directory "xml/'layout'" where 'layout' is the name of a different Pixel layout, namely Inclined_ALternative, Inclined_Quads, and Inclined_Duals. They have the following generic files: 
 
```bash
Materials.xml
PixelBarrel.xml
PixelModules.xml
PixelStave.xml
PixelSimpleService.xml
PixelEndcapDiskSupport.xml
```

In fact, they are just the symbolic links to the corresponding ATLAS layout files. For ATLAS, you will need to download these XML files and make the corresponding symbolic links back to the generic files in the "xml/'layout'" directory.
Typically, the XML files can be downloaded from the "share" directory of the [InDetTrackingGeometryXML package](https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetDetDescr/InDetTrackingGeometryXML/trunk/share?order=name) (available for ATLAS members only).
 
# More explanation on codding

To visualize the layout, use the files such as ShowPixelBarrel.cxx,  ShowPixelEndcap.cxx etc.
The file names start from the string "Show" and have a single method:


```c++
process(InDet::XMLReaderSvc& reader, TGeoVolume* top, TGeoManager* geom, int complexity, string infile)
```


S.Chekanov (ANL)

A.Fischer 
 


