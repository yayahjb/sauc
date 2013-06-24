#
#  Makefile for SAUC
#
#  Herbert J. Bernstein, Bernstein + Sons
#  Lawrence C. Andrews, Micro Encoder Inc.
#
#  1 July 2012
#  Rev 5 November 2012
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
#  SEARCHURL IS THE URL FOR NEW SEARCH
#
SEARCHURL	?=	http://$(HTTPDSERVER)/sauc#search
#
#  BINDEST is the installation directory for the executable
#  of sauc
BINDEST		?=	$(HOME)/bin
#
#  CGIBIN is the installation directory for the cgi-bin script
#  iterate.csh
CGIBIN		?=	$(HOME)/public_html/cgi-bin
#
#  CGIBINEXT is the external name of the directory for the
#  cgi-bin script sauc.csh
CGIBINEXT	?=	/cgi-bin
#
#  HTDOCS is the installation directory for the HTML document
#  sauc.html
HTDOCS		?=	$(HOME)/public_html/sauc
#
#  HTDOCSEXT is the external name of the directory for the 
#  HTML document sauc.html
HTDOCSEXT   ?=   /sauc
#
#
#  Default compile flag definition to select debug mode under unix
CXXFLAGS ?= -Wall -O3 -DUSE_LOCAL_HEADERS -g
CXX	?=	g++
#
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
MATHSCRIBETARBALLURL ?= http://downloads.sf.net/iterate/$(MATHSCRIBETARBALL)
MATHSCRIBEURL = http://$(HTTPDSERVER)$(HTDOCSEXT)/$(MATHSCRIBEPATH)

#
# SAUC URLS
#
SAUCVERSION = 0.6
SAUCTARBALLURL = http://downloads.sf.net/iterate/sauc-$(SAUCVERSION).tar.gz
SAUCZIPURL = http://downloads.sf.net/iterate/sauc-$(SAUCVERSION).zip


CGIPATH		?=	http://$(HTTPDSERVER)$(CGIBINEXT)
BINPATH		?=	$(BINDEST)/sauc
HTFLAGS 	=	-DCGIBIN=$(CGIPATH) \
		-DHTTPDSERVER=$(HTTPDSERVER) \
		-DMATHSCRIBEURL=$(MATHSCRIBEURL) \
		-DSAUCTARBALLURL=$(SAUCTARBALLURL) \
		-DSAUCZIPURL=$(SAUCZIPURL) \
		-DHTDOCS=$(HTDOCS)

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
		@/bin/echo "CXX  ----------- currently:" $(CXX)
		@/bin/echo "CXXFLAGS  ------ currently:" $(CXXFLAGS)
		@/bin/echo ""
		@/bin/echo "**************************************"
		@/bin/echo "* Then:                              *"
		@/bin/echo "*     make edit_done                 *"
		@/bin/echo "*     make install                   *"
		@/bin/echo "**************************************"
#
edit_done:	sauc sauc.html sauc.csh
		touch edit
#
clean:
		-@rm -f edit
		-@rm -f sauc.html
		-@rm -f sauc
		-@rm -f *.o
		-@rm -f sauc.csh
		-@rm -f *.bak
#
sauc.html:	sauc.html.m4 Makefile $(MATHSCRIBEPATH) gpl.txt lgpl.txt
		m4 $(HTFLAGS) < sauc.html.m4 > sauc.html
#
sauc.csh:	sauc.csh.m4 Makefile
		m4 -DSEARCHURL=$(SEARCHURL) \
		-DBINPATH=$(BINPATH) \
		-DSEARCHURL=$(SEARCHURL)\
		-DHTDOCS=$(HTDOCS)\
		< sauc.csh.m4 > sauc.csh
#
install:	edit_done sauc sauc.csh sauc.html \
		$(MATHSCRIBEPATH) gpl.txt lgpl.txt
		-mkdir -p $(BINDEST)
		-mkdir -p $(CGIBIN)
		-mkdir -p $(HTDOCS)
		chmod 755 sauc
		chmod 755 sauc.csh
		cp sauc $(BINDEST)
		cp sauc.csh $(CGIBIN)
		chmod 755 $(CGIBIN)/sauc.csh
		cp sauc.html $(HTDOCS)
		cp gpl.txt $(HTDOCS)
		cp lgpl.txt $(HTDOCS)
		cp *.csv $(HTDOCS)
		cp *.dmp $(HTDOCS)
		ln -f -s $(HTDOCS)/sauc.html $(HTDOCS)/index.html
		cp -r $(MATHSCRIBEPATH) $(HTDOCS)/$(MATHSCRIBEPATH)

#		
sauc: \
	BasicDistance.cpp    \
	BasicDistance.h    \
	Cell.cpp \
	Cell.h \
	sauc.cpp \
	NCDist.h \
	Reducer.cpp \
	Reducer.h \
	V7.cpp \
	V7.h \
	unitcell.h \
	rhrand.h \
	triple.h \
	TNear.h
	$(CXX) $(CXXFLAGS) -o sauc \
	BasicDistance.cpp    \
	Cell.cpp \
	sauc.cpp \
	Reducer.cpp \
	V7.cpp


$(MATHSCRIBETARBALL):
		wget http://downloads.sf.net/iterate/$(MATHSCRIBETARBALL)

$(MATHSCRIBEPATH): $(MATHSCRIBETARBALL)
		gunzip < $(MATHSCRIBETARBALL) | tar xvf -
		chmod -R 755 $(MATHSCRIBEPATH)
		touch $(MATHSCRIBEPATH)
