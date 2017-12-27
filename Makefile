#
#  Makefile for SAUC
#
#  Herbert J. Bernstein, Bernstein + Sons
#  Lawrence C. Andrews, Micro Encoder Inc.
#
#  1 July 2012
#  Rev 13 July 2013
#  Rev 21 April 2014
#  Rev 142 August 2015
#  Rev 17 Dec 2017
#
#  Modify the following definitions for your system
#
#  HTTPDSERVER is the name of the server on which the
#  installation is being made
#
#  *************************************************
#  *** YOU MUST CHANGE THIS DEFINITION TO PERMIT ***
#  ***              REMOTE ACCESS                ***
#  *************************************************
#
HTTPDSERVER	?=	HOST.DOMAIN/~$(USER)
#
#  SAUCVERSION is used to allow multiple co-existing versions
#
SAUCVERSION = 1.0.2
#
#  SEARCHURL IS THE URL FOR NEW SEARCH
#
SEARCHURL	?=	http://$(HTTPDSERVER)/sauc-$(SAUCVERSION)#search
#
#  BINDEST is the installation directory for the executable
#  of sauc
BINDEST		?=	$(HOME)/bin
#
#  CGIBIN is the installation directory for the cgi-bin script
#  iterate.csh
CGIBIN		?=	$(HOME)/public_html/cgi-bin
#
#  CGIMETHOD is either GET or POST
#
CGIMETHOD	?=	POST
#
#  CGIBINEXT is the external name of the directory for the
#  cgi-bin script $(SAUCCGI)
CGIBINEXT	?=	/cgi-bin
#
#  HTDOCS is the installation directory for the HTML document
#  $(SAUCHTML)
HTDOCS		?=	$(HOME)/public_html/sauc-$(SAUCVERSION)
#
#  HTDOCSEXT is the external name of the directory for the 
#  HTML document $(SAUCHTML)
HTDOCSEXT   ?=   /sauc-$(SAUCVERSION)
#
#  PDBCELLINDEXURL is the URL from which to retrieve the
#  fixed-field PDB cell index
PDBCELLINDEXURL ?= ftp://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx
PDBENTRIESURL ?= ftp://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx
#
#  Default compile flag definition to select debug mode under unix
CXXFLAGS ?= -Wall -O0 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -mfpmath=sse -msse2
CFLAGS ?= -Wall -O0 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -mfpmath=sse -msse2
#CXXFLAGS ?= -Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -mfpmath=sse -msse2
#CFLAGS ?= -Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=8 -mfpmath=sse -msse2
CXX	?=	g++
CC	?=	gcc
#
#  ZINCURL is the github URL for ZINC
#
ZINCURL	?= https://github.com/yayahjb/Zinc.git
#
#  CIFTBXURL is the github URL for ciftbx
#
CIFTBXURL  ?= https://github.com/yayahjb/ciftbx.git

#
#  Fortran compile (needed for database updates)
FC	=	gfortran
#
#
#  You should not have to edit below this line
#********************************************************************
#
#
# mathscribe is used by the web page to set formulae
#
MATHSCRIBEVERSION ?= 0.2.0
MATHSCRIBEPATH ?= mathscribe-$(MATHSCRIBEVERSION)
MATHSCRIBETARBALL ?= $(MATHSCRIBEPATH).tar.gz
MATHSCRIBETARBALLURL ?= http://downloads.sf.net/project/iterate/external_packages/$(MATHSCRIBETARBALL)
MATHSCRIBEURL = http://$(HTTPDSERVER)$(HTDOCSEXT)/$(MATHSCRIBEPATH)
#
# SAUC URLS
#
SAUCTARBALLURL = http://downloads.sf.net/iterate/sauc-$(SAUCVERSION).tar.gz
SAUCZIPURL = http://downloads.sf.net/iterate/sauc-$(SAUCVERSION).zip
SAUCHTML = sauc-$(SAUCVERSION).html
SAUCCGI = sauc-$(SAUCVERSION).csh
SAUCEXE = sauc-$(SAUCVERSION).exe

COD ?= .




CGIPATH		?=	http://$(HTTPDSERVER)$(CGIBINEXT)
BINPATH		?=	$(BINDEST)/$(SAUCEXE)
HTFLAGS 	=	-DCGIBIN=$(CGIPATH) \
		-DHTTPDSERVER=$(HTTPDSERVER) \
		-DMATHSCRIBEURL=$(MATHSCRIBEURL) \
		-DSAUCTARBALLURL=$(SAUCTARBALLURL) \
		-DSAUCZIPURL=$(SAUCZIPURL) \
		-DHTDOCS=$(HTDOCS) \
		-DCGIMETHOD=$(CGIMETHOD) \
	    -DSAUCHTML=$(SAUCHTML) \
	    -DSAUCCGI=$(SAUCCGI) \
	    -DSAUCEXE=$(SAUCEXE)

CGIFLAGS    =   -DSEARCHURL=$(SEARCHURL) \
	    -DBINPATH=$(BINPATH) \
	    -DSEARCHURL=$(SEARCHURL)\
	    -DHTDOCS=$(HTDOCS)\
	    -DCGIMETHOD=$(CGIMETHOD)\
	    -DSAUCHTML=$(SAUCHTML) \
	    -DSAUCCGI=$(SAUCCGI) \
	    -DSAUCEXE=$(SAUCEXE)

UPDFLAGS    =   -DSAUCDIR=$(PWD) \
	    -DHTTPDSERVER=$(HTTPDSERVER) \
	    -DSEARCHURL=$(SEARCHURL)\
	    -DCGIPATH=$(CGIPATH)\
	    -DHTDOCS=$(HTDOCS)\
	    -DCGIBIN=$(CGIBIN)\
	    -DCGIMETHOD=$(CGIMETHOD)\
	    -DBINDEST=$(BINDEST)\
	    -DPDBCELLINDEXURL=$(PDBCELLINDEXURL)\
	    -DPDBENTRIESURL=$(PDBENTRIESURL)\
	    -DSAUCHTML=$(SAUCHTML) \
	    -DSAUCCGI=$(SAUCCGI) \
	    -DSAUCEXE=$(SAUCEXE)


SAVEDB		=	./saves
NEWDB		=	./newdb


DMPFILES	=  CODentries.dmp  PDBcells.dmp    \
		sauc_NT_L1_ckp.dmp  sauc_NT_NCDist_ckp.dmp  \
		PDBentries.dmp  sauc_NT_L2_ckp.dmp  sauc_NT_V7_ckp.dmp

OBIGFILES	=  crystal.idx  PDBcelldatabase.csv  \
		cod.tsv         entries.idx  \
		PDBcelldatabase.tsv  

BIGFILES	= $(DMPFILES) $(OBIGFILES)

$(DMPFILES):	CODentries.dmp.bz2  PDBcells.dmp.bz2 \
		sauc_NT_L1_ckp.dmp.bz2  sauc_NT_NCDist_ckp.dmp.bz2 \
		PDBentries.dmp.bz2  sauc_NT_L2_ckp.dmp.bz2  sauc_NT_V7_ckp.dmp.bz2
		bunzip2 < $@.bz2 > $@

$(OBIGFILES):	crystal.idx.bz2  PDBcelldatabase.csv.bz2 \
		cod.tsv.bz2         entries.idx.bz2 \
		PDBcelldatabase.tsv.bz2  
		bunzip2 < $@.bz2 > $@


all:		edit
		@/bin/echo "  make edit_done   to build the kit after edits"
		@/bin/echo "  make install     to install SAUC program and web site"
		@/bin/echo "  make clean       to start over"
		@/bin/echo "  make updatedb    to update the database"
		
#
#
edit:	
		@/bin/echo "**************************************"
		@/bin/echo "* You must edit Makefile before      *"
		@/bin/echo "* installing SAUC                    *"
		@/bin/echo "**************************************"
		@/bin/echo "You must define the following in Makefile"
		@/bin/echo "or as environment variables:"
		@/bin/echo ""
		@/bin/echo "HTTPDSERVER ---- currently:" $(HTTPDSERVER)
		@/bin/echo "SEARCHURL ------ currently:" $(SEARCHURL)
		@/bin/echo "BINDEST -------- currently:" $(BINDEST)
		@/bin/echo "HTDOCS  -------- currently:" $(HTDOCS)
		@/bin/echo "CGIPATH  ------- currently:" $(CGIPATH)
		@/bin/echo "CGIBIN --------- currently:" $(CGIBIN)
		@/bin/echo "CGIMETHOD ------ currently:" $(CGIMETHOD)
		@/bin/echo "CXX  ----------- currently:" $(CXX)
		@/bin/echo "CXXFLAGS  ------ currently:" $(CXXFLAGS)
		@/bin/echo ""
		@/bin/echo "**************************************"
		@/bin/echo "* Then:                              *"
		@/bin/echo "*     make edit_done                 *"
		@/bin/echo "*     make install                   *"
		@/bin/echo "**************************************"

#
edit_done:	$(SAUCEXE) $(SAUCHTML) $(SAUCCGI) updatedb.csh sauc_psm_files_create $(BIGFILES)
		touch edit
#
clean:
		-@rm -f edit
		-@rm -f $(SAUCHTML)
		-@rm -f $(SAUCEXE)
		-@rm -f *.o
		-@rm -f $(SAUCCGI)
		-@rm -f *.bak
		-@rm -rf Zinc
		-@rm -rf ciftbx
		-@rm -f cif2cif
		-@rm -f idx2tsv
		-@rm -f $(BIGFILES)
#
$(SAUCHTML):	sauc.html.m4 Makefile $(MATHSCRIBEPATH) gpl.txt lgpl.txt
		m4 $(HTFLAGS) < sauc.html.m4 > $(SAUCHTML)
#
$(SAUCCGI):	sauc.csh.m4 Makefile edit
		m4 $(CGIFLAGS) < sauc.csh.m4 > $(SAUCCGI)
#
updatedb.csh:	updatedb.csh.m4 Makefile edit
		m4 $(UPDFLAGS) < updatedb.csh.m4 > updatedb.csh
		chmod 755 updatedb.csh
#
install:	edit $(SAUCEXE) $(SAUCCGI) $(SAUCHTML) \
		$(MATHSCRIBEPATH) gpl.txt lgpl.txt \
		$(BIGFILES)
		-mkdir -p $(BINDEST)
		-mkdir -p $(CGIBIN)
		-mkdir -p $(HTDOCS)
		chmod 755 $(SAUCEXE)
		chmod 755 $(SAUCCGI)
		cp $(SAUCEXE) $(BINDEST)
		cp $(SAUCEXE) $(CGIBIN)
		cp $(SAUCCGI) $(CGIBIN)
		chmod 755 $(CGIBIN)/$(SAUCCGI)
		cp $(SAUCHTML) $(HTDOCS)
		cp gpl.txt $(HTDOCS)
		cp lgpl.txt $(HTDOCS)
		cp *.tsv $(HTDOCS)
		cp *.dmp $(HTDOCS)
		cp entries.idx $(HTDOCS)
		ln -f -s $(HTDOCS)/$(SAUCHTML) $(HTDOCS)/index.html
		cp -r $(MATHSCRIBEPATH) $(HTDOCS)/$(MATHSCRIBEPATH)

#		
$(SAUCEXE): \
	BasicDistance.cpp    \
	BasicDistance.h    \
	Cell.cpp \
	Cell.h \
	fgetln.c \
	sauc.cpp \
	NCDist.h \
	Reducer.cpp \
	Reducer.h \
	V7.cpp \
	V7.h \
	unitcell.h \
	rhrand.h \
	triple.h \
	TNear.h \
	pststrmgr.c \
	pststrmgr.h
	$(CC) $(CFLAGS) -c pststrmgr.c
	$(CXX) $(CXXFLAGS) -o $(SAUCEXE) \
	BasicDistance.cpp    \
	Cell.cpp \
	fgetln.c \
	sauc.cpp \
	Reducer.cpp \
	V7.cpp \
	pststrmgr.o -lpthread

sauc_psm_files_create: \
	sauc_psm_files_create.c \
	sauc_psm_files_create.h \
	    pststrmgr.c \
	    pststrmgr.h \
	fgetln.c
	$(CC) $(CFLAGS) -c pststrmgr.c
	$(CC) $(CFLAGS) -o sauc_psm_files_create \
	sauc_psm_files_create.c fgetln.c \
	pststrmgr.o -lpthread

Zinc:
	git clone $(ZINCURL)

Zinc/src/cifZinc:	Zinc
	cd Zinc/src; make cifZinc

ciftbx:
	git clone $(CIFTBXURL)

cif2cif:  ciftbx
	cd ciftbx/cif2cif.src;  make clean; make cif2cif
	mv ciftbx/cif2cif.src/cif2cif .

getcodfields:	getcodfields.c pststrmgr.c pststrmgr.h fgetln.c
	$(CC) $(CFLAGS) -o getcodfields getcodfields.c fgetln.c pststrmgr.c

$(COD)/cif: load_cod_cifs	
	mkdir -p $(COD)/cif; rsync -avz --delete rsync://www.crystallography.net/cif/ $(COD)/cif/
	touch $(COD)/cif

cod.tsv.bz2:	$(COD)/cif getcodfields exdata.sh coddata.sh Zinc/src/cifZinc cif2cif $(SAVEDB)
	-cp cod.tsv.bz2 $(SAVEDB)
	./coddata.sh
	cat cod_*.tsv | bzip2 >  cod.tsv.bz2

$(MATHSCRIBETARBALL):
	wget --no-check-certificate $(MATHSCRIBETARBALLURL)

$(MATHSCRIBEPATH): $(MATHSCRIBETARBALL)
	gunzip < $(MATHSCRIBETARBALL) | tar xvf -
	chmod -R 755 $(MATHSCRIBEPATH)
	touch $(MATHSCRIBEPATH)
	    
idx2tsv:    idx2tsv.f
	$(FC) -o idx2tsv idx2tsv.f

$(NEWDB)/crystal.idx: $(NEWDB)
	(cd $(NEWDB); wget -N $(PDBCELLINDEXURL) )

$(NEWDB)/entries.idx: $(NEWDB)
	(cd $(NEWDB); wget -N $(PDBENTRIESURL) )

updatedb:   $(NEWDB)/crystal.idx $(NEWDB)/entries.idx idx2tsv $(SAVEDB) \
	    $(NEWDB) $(SAUCEXE) cod.tsv sauc_psm_files_create \
	    PDBcelldatabase.tsv PDBentries.dmp PDBcells.dmp CODentries.dmp
	-cp PDBcelldatabase.tsv $(SAVEDB)/
	-cp *.dmp.bz2 $(SAVEDB)/
	-(cd $(NEWDB);rm -f *.dmp result*)
	cp cod.tsv $(NEWDB)/
	cp PDBentries.dmp $(NEWDB)/
	cp PDBcells.dmp $(NEWDB)/
	cp CODentries.dmp $(NEWDB)/
	(cd $(NEWDB);../idx2tsv < crystal.idx | \
	  bzip2 >  PDBcelldatabase.tsv.bz2; bunzip2 <  PDBcelldatabase.tsv.bz2 > PDBcelldatabase.tsv)
	(cd $(NEWDB);../sauc_psm_files_create PDB)
	(cd $(NEWDB);../sauc_psm_files_create COD)
	(SAUC_BATCH_MODE=1;export SAUC_BATCH_MODE;cd $(NEWDB);../$(SAUCEXE) < ../rebuild.inp)
	(cd $(NEWDB);grep "1O51" resultL1)
	(cd $(NEWDB);grep "1O51" resultL2)
	(cd $(NEWDB);grep "1O51" resultNCDist)
	(cd $(NEWDB);grep "1O51" resultV7)
	(cd $(NEWDB);bzip2 *.dmp)
	(cd $(NEWDB);date > last_update) 
	touch updatedb

last_update:	$(NEWDB)/last_update updatedb
	cp $(NEWDB)/*.dmp.bz2 .
	cp $(NEWDB)/*.tsv.bz2 .
	cp $(NEWDB)/last_update .
	cp $(NEWDB)/entries.idx .
	touch last_update

$(NEWDB)/last_update:
	touch $(NEWDB)/last_update

$(SAVEDB):
	mkdir -p $(SAVEDB)

$(NEWDB):
	mkdir -p $(NEWDB)
