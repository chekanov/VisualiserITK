# S.Chekanov

ifndef ROOTSYS 
$(error ROOTSYS env variable is not set. Run setup.sh first)
endif

ifndef XercesC_HOME 
$(error XercesC_HOME env variable is not set. Run setup.sh first)
endif


# ANL analysis tutorial 
include ${ROOTSYS}/etc/Makefile.arch


# Root variables
ROOTCFLAGS    = $(shell root-config --nonew --cflags)
ROOTLIBS      = $(shell root-config --nonew --libs)
ROOTGTTLIBS   = $(shell root-config --nonew --glibs)
# Assign or add variables
CXXFLAGS     += $(ROOTCFLAGS)
LIBS         += $(ROOTLIBS) -lGeom -lMathMore -lEve
LIBS         +=-L$(XercesC_HOME)/lib -lxerces-c
# LIBS         +=-L$(CLHEP_DIR)/lib -lCLHEP-2.1.4.1
 
 
# LIBS         += -L./ApplyJetCalibration/StandAlone   -lApplyJetCalibration

SOURCE_FILES := $(shell ls -1 itkvis.cxx)
SOURCE_FILES += $(shell ls -1 src/*.cxx)

INCLUDE1= -I./inc
INCLUDE2= -I. -I$(XercesC_HOME)/include
 


# build object files 
objects       = $(patsubst %.cxx,%.o,$(SOURCE_FILES))


%.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(INCLUDE1) $(INCLUDE2) $(INCLUDE3) -o $@ -c $<

Tasks:    itkvis


tar:
	rm -f ana.tgz; cd ../; tar -cvzf ana.tgz --exclude='*.o' --exclude='*.root' \
	--exclude='*.log'  --exclude='*.tup' --exclude='*.eps' --exclude='*.png' --exclude='*.log.gz'\
	--exclude='itkvis' --exclude='*.root.log.gz' --exclude='*.so' --exclude='*~' --exclude='*.pyc' ana; \
        mv ana.tgz ana/; cd ana; 


itkvis: $(objects)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
clean:
	        @rm -f *.o  itkvis *~ src/*.o src/*~ inc/*~; echo "Clear.." 
