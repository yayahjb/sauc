#!/bin/sh
HTTPDSERVER=http://flops.arcib.org:8084;export HTTPDSERVER
HTDOCS=/var/www/html/sauc-1.0.2;export HTDOCS;
SEARCHURL=http://flops.arcib.org:8084/sauc-1.1.1/;export SEARCHURL;
CGIPATH=http://flops.arcib.org:8084/cgi-bin;export CGIPATH;
CGIBIN=/var/www/cgi-bin;export CGIBIN;
CGIMETHOD=GET;export CGIMETHOD;
BINDEST=/var/www/cgi-bin;export BINDEST;
PDBCELLINDEXURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://ftp.wwpdb.org/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
#PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
#PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
export CC=gcc-4.8; export CXX=g++-4.8; export FC=gfortran-4.8
export CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -fopenmp  -ftree-parallelize-loops=12 -mfpmath=sse -msse2 -I.";
