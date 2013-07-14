#!/bin/sh
HTTPDSERVER=`hostname -f`/~${USER};export HTTPDSERVER
SEARCHURL=http://`hostname -f`/~${USER}/sauc;export SEARCHURL
CGIPATH=http://`hostname -f`/cgi-bin/cgiwrap/${USER};export CGIPATH
PDBCELLINDEXURL=http://www.bmrb.wisc.edu/ftp/pub/pdb/derived_data/index/crystal.idx;export PDBCELLINDEXURL;
CGIMETHOD=GET;export CGIMETHOD;
