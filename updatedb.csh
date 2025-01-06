#!/bin/csh
#
#  Example script for updating the cell database
#  from the PDB crystal.idx cell index
#
#  Assumes a working sauc kit in SAUCDIR
#
cd /home/yaya/sauc_26Dec24/sauc
setenv HTTPDSERVER http://blondie.arcib.org:8083
setenv SEARCHURL http://blondie.arcib.org:8083/~yaya/sauc/
setenv CGIPATH http://blondie.arcib.org:8083/~yaya/cgi-bin
setenv PDBCELLINDEXURL https://files.rcsb.org/pub/pdb/derived_data/index/crystal.idx
setenv PDBENTRIESURL https://files.rcsb.org/pub/pdb/derived_data/index/entries.idx
setenv CGIMETHOD GET
setenv CGIBIN /home/yaya/public_html/cgi-bin
setenv BINDEST /home/yaya/public_html/cgi-bin
setenv HTDOCS /home/yaya/public_html/sauc
if ( -e crystal.idx ) then
  rm -f newdb/crystal.idx
  make newdb/crystal.idx
  diff crystal.idx newdb/crystal.idx > /dev/null
  if ( $status ) then
    make last_update
    cp newdb/crystal.idx crystal.idx
    set check_update=1
  else
    set check_update=0
    make sauc-1.2.1.exe
    ./sauc-1.2.1.exe < rebuild.inp
    touch resultL1
    touch resultL2
    touch resultNCDist
    touch resultV7
    touch resultD7
    touch resultS6
    touch resultDC7unsrt
    grep "1O51" resultL1
    if ( $status ) set check_update=1
    grep "1O51" resultL2
    if ( $status ) set check_update=1
    grep "1O51" resultNCDist
    if ( $status ) set check_update=1
    grep "1O51" resultV7
    if ( $status ) set check_update=1
    grep "1O51" resultD7
    if ( $status ) set check_update=1
    grep "1O51" resultS6
    if ( $status ) set check_update=1
    grep "1O51" resultDC7unsrt
    if ( $status ) set check_update=1
  endif
else
  make last_update
  cp newdb/crystal.idx crystal.idx
  set check_update=1
endif
if ( $check_update) then
  (setenv SAUC_BATCH_MODE 1;./sauc < rebuild.inp)
  make install
  rm -f newdb/*.dmp
  rm -f newdb/*.csv
  rm -f newdb/*.idx
  rm -f newdb/result*
endif
