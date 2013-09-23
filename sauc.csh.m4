`#!/bin/csh
# sauc.csh
#
# Keith J. McGill
#
# September 23, 2013
#
# This is a service script for the sauc.html web page
# It must be placed in an appropriate cgi-bin directory on
# the server pointed to by sauc.html
#
#
# Set the following to the best available search URL
set searchurl="'SEARCHURL()`"
#
# To operate correctly, the programs tr and sed must be in the
# default path and the /bin/echo version of echo must follow
# system V conventions sufficiently to produce an empty line
# call, below
#
/bin/echo "Content-type: text/html"
/bin/echo 
echo "<head>"
echo "<title>Search of Alternate Unit Cells (SAUC)"
echo "</title>"
echo "</head>"
echo ''`<body><font face="Arial,Helvetica,Times">''`
'ifelse(CGIMETHOD(),`GET',`echo $QUERY_STRING |')` tr ''`\&''` ''`\n''`  |sed "s/^./set &/" | sed "s/%2B/+/g" > /tmp/outstr$$
#cat /tmp/outstr$$
source /tmp/outstr$$
rm /tmp/outstr$$
echo ''`<center>''`
echo ''`| <a href="#Results">GO TO RESULTS</a>''`
echo ''`| <a href="''`${searchurl}''`">NEW SEARCH</a> |</center>''`
echo ''`<hr />''`
echo "<h1 align=center>SAUC</h1>"
echo "<h3 align=center>Search of Alternate Unit Cells</h3>"
echo ''`<center>''`
echo "Copyright Keith J. McGill 2013"
echo ''`</center>''`
echo "<hr />"
echo "" >/tmp/instr$$
echo ${Centering} " " >> /tmp/instr$$
echo "<P>| Lattice Centering: " $Centering 
echo $A " " >>/tmp/instr$$
echo $B " " >>/tmp/instr$$
echo $C " " >>/tmp/instr$$
echo $Alpha " " >>/tmp/instr$$
echo $Beta " " >>/tmp/instr$$
echo $Gamma " " >>/tmp/instr$$
echo "| Cell: " A:$A B:$B C:$C Alpha:$Alpha Beta:$Beta Gamma:$Gamma
echo ${Algorithm} " "  >> /tmp/instr$$
echo "| Algorithm: "
if ($Algorithm == 1) then
	echo "L1"
else if ($Algorithm == 2) then
	echo "L2"
else if ($Algorithm == 3) then
	echo "NCDist"
else
	echo "V7"
endif
echo ${Similarity} >> /tmp/instr$$
echo "| Similarity: "
if ($Similarity == 1) then
	echo "Nearest"
else if ($Similarity == 2) then
	echo "Sphere"
else
        echo "Range"
endif
if ($Similarity == 2) then
	echo ${RangeSphere} >> /tmp/instr$$
	echo "| Range of S: " $RangeSphere " |"
else if ($Similarity == 3) then
	echo ${RangeA} >> /tmp/instr$$
	echo "| Range of A: " $RangeA
	echo ${RangeB} >> /tmp/instr$$
	echo "| Range of B: " $RangeB
	echo ${RangeC} >> /tmp/instr$$
	echo "| Range of C: " $RangeC
	echo ${RangeAlpha} >> /tmp/instr$$
	echo "| Range of Alpha: " $RangeAlpha
	echo ${RangeBeta} >> /tmp/instr$$
	echo "| Range of Beta: " $RangeBeta
	echo ${RangeGamma} >> /tmp/instr$$
	echo "| Range of Gamma: " $RangeGamma " |"
else
	echo " |"
endif
echo "" >> /tmp/instr$$
echo "4 " >> /tmp/instr$$
echo "" >>/tmp/instr$$
echo ''`<p><hr /><p><h2><a name="Results"></a>Results of SAUC Run</h2>''`
setenv SAUC_BATCH_MODE YES
echo "<PRE>"
cd' HTDOCS()`
'BINPATH()` < /tmp/instr$$
rm /tmp/instr$$
#cat /tmp/instr$$ 
echo "</pre>"
echo ''`<p>''`
echo "<hr />"
echo ''`<center>''`
echo ''`| <a href="#Results">GO TO RESULTS</a>''`
echo ''`| <a href="''`${searchurl}''`">NEW SEARCH</a> |</center>''`
echo "</font>"
echo "</body>"'
