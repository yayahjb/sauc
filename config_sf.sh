#!/bin/sh
HTTPDSERVER=iterate.sf.net;export HTTPDSERVER
HTDOCS=/home/project-web/iterate/htdocs/sauc-1.0.0;export HTDOCS;
SEARCHURL=http://iterate.sf.net/sauc-1.0.0;export SEARCHURL;
CGIPATH=http://iterate.sf.net/cgi-bin;export CGIPATH;
CGIBIN=/home/project-web/iterate/cgi-bin;export CGIBIN;
CGIMETHOD=GET;export CGIMETHOD;
BINDEST=/home/project-web/iterate/cgi-bin;export BINDEST;
PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
PDBENTRIESURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/entries.idx;export PDBENTRIESURL;
export CXXFLAGS="-Wall -O3 -DUSE_LOCAL_HEADERS -g -I$HOME/include"
