`#!/bin/csh
#
#  Example script for updating the cell database
#  from the PDB crystal.idx cell index
#
#  Assumes a working sauc kit in SAUCDIR
#
cd 'SAUCDIR()`
setenv HTTPDSERVER' HTTPDSERVER()`
setenv SEARCHURL' SEARCHURL()`
setenv CGIPATH' CGIPATH()`
setenv PDBCELLINDEXURL' PDBCELLINDEXURL()`
setenv PDBENTRIESURL' PDBENTRIESURL()`
setenv CGIMETHOD' CGIMETHOD()`
setenv CGIBIN' CGIBIN()`
setenv BINDEST' BINDEST()`
setenv HTDOCS' HTDOCS()`
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
    make 'SAUCEXE()`
    ./'SAUCEXE()` < rebuild.inp
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
endif'
