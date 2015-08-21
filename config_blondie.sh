#!/bin/sh
HTTPDSERVER=http://blondie.ddns.net:8083;export HTTPDSERVER
HTDOCS=/var/www/sauc-0.9.0;export HTDOCS;
SEARCHURL=http://blondie.ddns.net:8083/sauc-0.9.0/;export SEARCHURL;
CGIPATH=http://blondie.ddns.net:8083/cgi-bin;export CGIPATH;
CGIBIN=/var/www/cgi-bin;export CGIBIN;
CGIMETHOD=GET;export CGIMETHOD;
BINDEST=/var/www/cgi-bin;export BINDEST;
PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
export CXXFLAGS="-Wall -g -O3 -DUSE_LOCAL_HEADERS -g -I. -I/usr/local/include -L/usr/local/lib"
export LD_LIBRARY_PATH="/usr/local/lib";
