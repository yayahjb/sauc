#!/bin/sh
HTTPDSERVER=`hostname -f`/~${USER};export HTTPDSERVER
SEARCHURL=http://`hostname -f`/~${USER}/sauc;export SEARCHURL
CGIPATH=http://`hostname -f`/cgi-bin/cgiwrap/${USER};export CGIPATH

