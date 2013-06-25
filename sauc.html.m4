`<!doctype html public "-//IETF//DTD HTML 2.0//EN">
<HTML>
<HEAD>
	<meta charset="utf-8">
	
	<link rel="stylesheet" href="http://fonts.googleapis.com/css?family=UnifrakturMaguntia">
        <link rel="stylesheet" href="'MATHSCRIBEURL()`/jqmath-0.2.0.css">
	
	<script src="'MATHSCRIBEURL()`/jquery-1.4.3.min.js"></script>
	<script src="'MATHSCRIBEURL()`/jqmath-etc-0.2.0.min.js"></script>
	<!-- <script>M.MathPlayer = false;</script> -->

<!--
To use jqMath in an html file, use a text editor to copy the above lines to the beginning of
your file, remove any corresponding old tags, and save the file in UTF-8 format.

If you aren''`t writing in English, change "en" above to the correct language code, especially if
your language allows a comma as a decimal mark (instead of a period).

If you don''`t use Fraktur characters, you can remove the link on line 7, as this may speed up
download times slightly.

You may need to change each "http://arcib.dowling.edu/~mcgillk/bgaol/mathscribe-0.2.0" path above, depending on the directory structure
that you choose.

You can use a different version of jquery (1.0+) if you prefer.  We''`ve just provided this one
for your convenience.

If you need to use the \html macro, then uncomment the M.MathPlayer line.

The files jscurry-0.2.0.js, jscurry-0.2.0.min.js, and jqmath-0.2.0.js are provided just for your
information.  They are all included, and compressed, in jqmath-etc-0.2.0.min.js.
-->
<script 
language="javascript" type="text/javascript">
function pfloat(pfield){
    // validate for non-negative float   
    var charsAllowed="0123456789.+";
    var allowed;
    var plusfound;
    var dotfound;
    plusfound = 0;
    dotfound = 0;
    for(var i=0;i<pfield.value.length;i++){       
        allowed=false;
        for(var j=0;j<charsAllowed.length;j++){
            if( pfield.value.charAt(i)==charsAllowed.charAt(j) ){ 
               allowed=true;
               if (j == 11) {
                 plusfound++;
                 allowed=false;
                 plusfound--;
                 break;
               } else if (j == 10) {
                 dotfound++;
                 if (dotfound > 1) {
                   allowed=false;
                   dotfound--;
                   break;
                 }
               } 
               break;
            }
        }
        if(allowed==false){ pfield.value = pfield.value.replace(pfield.value.charAt(i),""); i--; }
    }
    return true;
}

function gfloat(field){
    // validate for float   
    var charsAllowed="0123456789.+-";
    var allowed;
    var plusminusfound;
    var dotfound;
    plusminusfound = 0;
    dotfound = 0;
    otherfound = 0;
    for(var i=0;i<field.value.length;i++){       
        allowed=false;
        for(var j=0;j<charsAllowed.length;j++){
            if( field.value.charAt(i)==charsAllowed.charAt(j) ){ 
               allowed=true;
               if (j == 11 || j == 12) {
                 if (otherfound>0 || dotfound>0 || plusminusfound>0) {
                   allowed=false;
                 } else {
                 plusminusfound++;
                 }
               } else if (j == 10) {
                 if (dotfound>0) {
                   allowed=false;
                 } else {
                 dotfound++;
                 } 
               } else {
                 otherfound++;
               } 
            }
        }
        if(allowed==false){ field.value = field.value.replace(field.value.charAt(i),""); i--; }
    }
    return true;
}
</script>

<TITLE>
Search of Alternate Unit Cells - SAUC
</TITLE> 
</HEAD> 
<BODY>
<font face="Arial,Helvetica,Times" size="3">
<center>
| <a href="#Search">Search</a>
| <a href="#Source">Source</a>
| <a href="#Introduction">Introduction</a>
| <a href="#History">History</a>
| <a href="#Background">Background</a>
| <a href="#Implementation1">Implementation - Distance</a>
| <a href="#Implementation2">Implementation - Searching</a>
| <a href="#Comparison">Comparison of Search Methods</a>
| <a href="#Tables">Tables</a>
| <a href="#References">References</a>
|
</center>
<hr />
<center>
<H1>SAUC</H1>
Search of Alternate Unit Cells
<br />Copyright Keith J McGill 2013
<br /><br />Keith J McGill, Mojgan Asadi, Maria Toneva Karakasheva, Lawrence C. Andrews, Herbert J. Bernstein
<br /><br />Dowling College and Micro Encoder Inc.
<br />Work Supported In Part By NIGMS
<br /><br />Contact Email: <A HREF=mailto:kjmcgill7@gmail.com>kjmcgill7@gmail.com</A>
<FORM method=POST ACTION="'CGIBIN()`/sauc.csh">
</center>
A crystallographic cell is a representation of a lattice, but each lattice can be represented
just as well by any of an infinite number of such unit cells. Searching
for matches to an experimentally determined crystallographic unit cell in a large
collection of previously determined unit cells is a useful verification step in synchrotron
data collection and can be a screen for &quot;similar&quot; structures, but it is more
useful to search for a match to the lattice represented by the experimentally determined
cell. For identification of substances with small cells, a unit cell match
may be sufficient for unique identification. Due to experimental error and multiple
choices of cells and differing choices of lattice centering representing the
same lattice, simple searches based on raw cell edges and angles can miss similarities
among lattices. A database of lattices using the G<sup>6</sup> representation of the
Niggli-reduced cell as the search key provides a more robust and complete search.
Searching is implemented by finding the distance from the probe cell to related
cells using a topological embedding of the Niggli reduction in G<sup>6</sup>, so that all cells
representing similar lattices will be found. Comparison of results with those from
older cell-based search algorithms suggests significant value in the new approach.
<center>
<br /><STRONG>
Please read the <a href="#notice">NOTICE</a> below before use of this web page
</STRONG>
<p>
<a name="Search"></a>
<INPUT type="submit">
<INPUT type="reset">
<br />
<input type=hidden name="OutputStyle" value="TEXT" />
<table border=2>
<tr><td valign=top>
<table>
<tr><th align="left">
Lattice Centering:</th>
</tr>
<tr>
<td> 
<SELECT name="Centering" size="9">
<option selected value="P">P (primitive)
<option value="A">A (a-centered)
<option value="B">B (b-centered)
<option value="C">C (c-centered)
<option value="F">F (all-faces-centered)
<option value="I">I (body-centered)
<option value="R">R (rhombohedral as hexagonal)
<option value="H">H (hexagonal)
<option value="V">V (raw g6 vector)
</SELECT>
</td>
</tr>
<th align="left">
Algorithm:</th>
<tr><td>
<input type="radio" name="Algorithm" value="1" />L1<br />
<input type="radio" name="Algorithm" value="2" />L2<br />
<input type="radio" name="Algorithm" value="3" /checked>NCDist<br />
<input type="radio" name="Algorithm" value="4" />V7<br />
</td></tr>
</table>
</td>
<td valign=top>
<table>
<tr><th align="left" colspan=4>
Cell Lengths and Angles:
</th>
</tr>
<tr>
<td>Length of A</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="A" 
VALUE="100.0" SIZE="9"></td> 
<td>Angle of Alpha</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="Alpha" 
VALUE="90.0" SIZE="9"></td>
</tr><tr>  
<td>Length of B</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="B" 
VALUE="100.0" SIZE="9"> </td>
<td>Angle of Beta</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="Beta" 
VALUE="90.0" SIZE="9"></td>
</tr><tr>
<td>Length of C</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="C" 
VALUE="100.0" SIZE="9"> 
<td>Angle of Gamma</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="Gamma" 
VALUE="90.0" SIZE="9"></td>
</tr>
<th align="left">
Similarity:</th>
<tr><td>
<input type="radio" name="Similarity" value="1" />Nearest<br />
<input type="radio" name="Similarity" value="2" /checked>Sphere<br />
Range of S<td><br /><br /><INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeSphere"VALUE="2.5" SIZE="9"></td>
</td></tr>
<tr><td>
<input type="radio" name="Similarity" value="3" />Range<br />
</td></tr>
<tr>
<td>Range of A</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeA" 
VALUE="1.0" SIZE="9"></td> 
<td>Range of Alpha</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="RangeAlpha" 
VALUE="1.0" SIZE="9"></td>
</tr><tr>  
<td>Range of B</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeB" 
VALUE="1.0" SIZE="9"> </td>
<td>Range of Beta</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="RangeBeta" 
VALUE="1.0" SIZE="9"></td>
</tr><tr>
<td>Range of C</td><td><INPUT TYPE="text" onchange="pfloat(this)" NAME="RangeC" 
VALUE="1.0" SIZE="9"> 
<td>Range of Gamma</td><td> <INPUT TYPE="text" onchange="gfloat(this)" NAME="RangeGamma" 
VALUE="1.0" SIZE="9"></td>
</tr>
</table>
</td>
</tr>
</table>
<INPUT type="hidden" NAME="Flush" VALUE="DUMMY">
<INPUT type="submit">
<INPUT type="reset">
</Form> <hr>
</center>
<a name="notice"><H2 align="center">NOTICE</H2></a>
<center>
<table border=0>
<tr><td width=200>
<font size="2">
<P>
You may redistribute this program under the terms of the <a href="gpl.txt">GPL</a>.
<p>
Alternatively you may redistribute the functions
and subroutines of this program as an API under the
terms of the <a href="lgpl.txt">LGPL</a>.
<p>
</td>
<td>
<div style="width:800px;height:300px;overflow:scroll;border:2px solid #0000FF;">
<font size="2">
<table border=2>
<tr>
<th align="center">GPL NOTICES</th>
<th align="center">LGPL NOTICES</th></tr>
<tr><td><font size="2">
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
(the License, or (at your option) any later version.
<p>
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
<p>
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307  USA</font>
</td>
<td><font size="2">
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
<p>
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
<p>
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
MA  02110-1301  USA</font>
</td>
</tr>
</table>


<P>
<STRONG>

Some of the software and documents included within this software package are the intellectual property of 
various parties, and placement in this package does not in anyway imply that any such rights have in any 
way been waived or diminished.

<P>

With respect to any software or documents for which a copyright exists, ALL RIGHTS ARE RESERVED TO THE 
OWNERS OF SUCH COPYRIGHT.

<P>

Even though the authors of the various documents and software found here have made a good faith effort to 
ensure that the documents are correct and that the software performs according to its documentation, and 
we would greatly appreciate hearing of any problems you may encounter, the programs and documents any 
files created by the programs are provided **AS IS** without any warrantee as to correctness, 
merchantability or fitness for any particular or general use.

<P>

THE RESPONSIBILITY FOR ANY ADVERSE CONSEQUENCES FROM THE USE OF PROGRAMS OR DOCUMENTS OR ANY FILE OR FILES 
CREATED BY USE OF THE PROGRAMS OR DOCUMENTS LIES SOLELY WITH THE USERS OF THE PROGRAMS OR DOCUMENTS OR 
FILE OR FILES AND NOT WITH AUTHORS OF THE PROGRAMS OR DOCUMENTS.

</STRONG>
</font>
</div>
</td>
</tr>
</table>
</center>
<P>
<hr>
</font>
<a name="source"></a>
<H2>Access to the source of SAUC</H2>

<P>
This program and related scripts are available as 
a <a href="'SAUCTARBALLURL()`">tarball</a>
or a 
<a href="'SAUCZIPURL()`">zip file</a>.
<br />The following information below is available as a PDF.
<p>

<a name="Introduction"></a>
<h2>Introduction</h2> 
<p>

(Andrews & Bernstein, 2012) introduced a topological embedding
of the Niggli &quot;cone&quot; of reduced cells with the goal of calculating
a meaningful distance between unit cells. In the second
paper of this series, the embedding was used to determine likely
Bravais lattices for a unit cell. Here we apply the embedding to
searching within a database for lattices &quot;close&quot; to the lattice of
a given probe cell.
<p>
A crystallographic cell is a representation of a lattice, but
each lattice can be represented just as well by any of an infinite
number of such unit cells. Searching for matches to an experimentally
determined crystallographic unit cell in a large collection
of previously determined unit cells is a useful verification
step in synchrotron data collection and can be a screen for &quot;similar&quot;
structures (Ramraj et al., 2011) (Mighell, 2002), but it is
more useful to search for a match to the lattice represented by
the experimentally determined cell, which may involve many
more cells. For identification of substances with small cells,
a unit cell match may be sufficient for unique identification
(Mighell, 2001).
<p>
Due to experimental error and multiple cells representing the
same lattice and differing choices of lattice centering, simple
searches based on raw cell edges and angles can miss similarities.
A database of lattices using the G<sup>6</sup> representation of the
Niggli-reduced cell as the search key provides a more robust
and complete search. Searching is implemented by finding the
distances from the probe cell to related cells using a topological
embedding of the cone of Niggli reduced cells in G<sup>6</sup>. Comparison
of results to those from older cell-based search algorithms
suggests significant value in the new approach.
<p>

<a name="History"></a>
<h2>History</h2>

<p>

Tabulations of data for the identification of minerals dates to
the 18th and 19th centuries. Data collected included interfacial
angles of crystals (clearly related to unit cell parameters) and
optical effects. See the historical review in (Burchard, 1998).
With the discovery of x-ray diffraction, those tables were supplanted
by new collections. Early compilations that included
unit cell parameters arranged for material identification were
&quot;Crystal Structures&quot; (Wyckoff, 1931), &quot;Crystal Data Determinative
Tables&quot; (Donnay, 1943), and Handbook for Metals and
Alloys (Pearson, 1958). Early computerized searches were created
by JCPDS in the mid-1960''`s (Johnson, 2013) and the
Cambridge Structural Data file and its search programs (Allen
et al., 1973).
<p>
Those first searches were sensitive to the issues of differing
equivalent presentations of the same lattice. The first effective
algorithm for resolving that issue was (Andrews et al., 1980)
using the V7 algorithm (NIH/EPA, 1980). Subsequently, other
programs using the V7 algorithm have been described (see
Table 1). The V7 algorithm has the advantage over simple
Niggli-reduction based cell searches of being stable under
experimental error. However, sensitivity to a change in an angle
is reduced as that angle nears 90 degrees.

<p>

<a name="Background"></a>
<h2>Background</h2>
<p>

An effective search method must find ways to search for related
unit cells, even when they appear to be tabulated in ways that
make them seem different. A trivial example is:<br/>
a = 10.0, b = 10.01, c = 20,
<br/>
&#945; = 65, &#946; = 75, 
&#947; = 90
<br/>versus<br/>
a = 10.0, b = 10.05, c = 20,
<br/>
&#945; = 75, &#946; = 65, 
&#947; = 90.
<p>
Clearly, these unit cells are almost identical, but simple tabulations
might separate them. A somewhat more complex example
includes the following primitive cells:<br/>
a = 3.1457, b = 3.1457, c = 3.1541,
<br/>
&#945; = 60.089, &#946; = 60.0887, 
&#947; = 60.104
<br/>versus<br/>
a = 3.1456, b = 3.1458, c = 3.1541,
<br/>
&#945; = 90.089, &#946; = 119.907, 
&#947; = 119.898.
<p>
Here the relationship is not as obvious. The embedding of
(Andrews & Bernstein, 2012) can be used to show that the
distance between these two cells is quite small in G<sup>6</sup> (0.004
&#8491;ngstrom units squared in G<sup>6</sup>).
<p>


<a name="Implementation1"></a>
<h2>Implementation - Distance</h2>
<p>
The program SAUC is structured to allow use of several alternative
metrics for searching among cells in an attempt to identify
cells representing similar lattices. To simplify comparisons
among results with the different metrics, all have been linearized
and normalized, i.e. converted to &#8491;ngstrom units and
scaled to be commensurate with the L<sub>2</sub> norm given below:
<p>
A simple L<sub>1</sub> or L<sub>2</sub> norm based on<br/>
[a, b, c, 
&#945;(b + c)/2, &#946;(a + c)/2, 
&#947;(a + b)/2]<br/>
with the distance scaled by 1/&#8730;(6) in the case of the L<sub>1</sub>
norm and unscaled in case of the L<sub>2</sub> norm. The angles
as assumed to be in radians and the edges in &#8491;ngstroms.
The angles were converted to &#8491;ngstroms by multiplying
by the average to the relevant edge lengths.
<p>
The square root of the BGAOL Niggli cone embedding
distance NCDist based on<br/>
[a<sup>2</sup>, b<sup>2</sup>, c<sup>2</sup>, 2bccos(&#945;), 2accos(&#946;), 2abcos(&#947;)]<br/>
with the distances scaled by 1/&#8730;(6) and divided by the
reciprocal of the average length of cell edges f. The
square root linearizes the metric to &#8491;ngstrom units.
<p>
The V7 distances based on individual components linearized
to &#8491;ngstrom units<br/>
[a, b, c, 1/a<sup>&#8727;</sup>, 1/b<sup>&#8727;</sup>, 1/c<sup>&#8727;</sup>,V<sup>1/3</sup>]<br/>
and scaled by &#8730;(6/7). V is the volume.
<p>
These metrics are applied to reduced primitive
cells [a, b, c, 
&#945;, &#946;, 
&#947;] and, when the reciprocal cell
[a<sup>&#8727;</sup>, b<sup>&#8727;</sup>, c<sup>&#8727;</sup>, 
&#945;<sup>&#8727;</sup>, &#946;<sup>&#8727;</sup>, 
&#947;<sup>&#8727;</sup>] is needed for the V7 metric, that cell
is also reduced.
<p>
In order to facilitate comparisons to older searches that just
consider simple ranges in [a, b, c, 
&#945;, &#946;, 
&#947;], an option for such
searches was also included in SAUC.
<h3>Validity of using the square root</h3>
The use of the square root on a metric preserves the triangle
inequality, which is important in order to preserve the metric
as a metric-space &quot;metric&quot;. The triangle inequality states
that for any triangle, the sum of the lengths of any two sides
is greater than the length of the third side. In metric space
terms, the metric d(x, y) of a metric space M satisfies d(x, z) &#8804;
d(x, y) + d(y, z), &#8704;x, y, z &#8712; M. Suppose a function f satisfies
the following conditions:<br/>
u &#8805; v &#8594; f(u) &#8805; f(v), &#8704;u, v<br/>
f(u + v) &#8804; f(u) + f(v), &#8704;u, v<br/>
then, if d(x,y) satisfies the triangle inequality, f(d(x,y))will also
satisfy the triangle inequality:<br/>
d(x, z) &#8804; d(x, y) + d(y, z)<br/>
&#8594; f(d(x, z)) &#8804; f(d(x, y) + d(y, z)) &#8804; f(d(x, y)) + f(d(y, z))
<p>
The square root satisfies the stated requirements. It is monotone,
and<br/>
&#8730;(u + v) &#8804; &#8730;(u) + &#8730;(v)<br/>
&#8596; u + v &#8804; (&#8730;(u) + &#8730;(v))<sup>2</sup> = u + v + 2&#8730;(uv)<br/>
which is clearly true.

<p>

<a name="Implementation2"></a>
<h2>Implementation - Searching</h2>
<p>
Range searching in amapped embedding needs to be done using
a nearest-neighbor algorithm (or &quot;post-office problem&quot; algorithm
(Knuth, 1973)). Exact matches are unlikely since most
unit cells representing lattices in a database are experimental,
and probe cells are also likely to be experimental data. Several
efficient algorithms are available; we have used an implementation
of neartree (Andrews, 2001).
<p>
The raw unit cell data is loaded into the tree once and serialized
to a dump file on disk; subsequent searches do not need to
wait for the O(Nlog(N)) tree build, which for the 70,000+ cells
from the PDB can take half an hour in the BGAOL NCDist metric.
The linearization makes the search space more compact and
reduces the tree depth, thereby speeding searches. Because the
PDB unit cell database contains many identical cells, we modified
NearTree to handle the duplicates in auxiliary lists, further
reducing the tree depth and speeding searches.

<p>

<a name="Comparison"></a>
<h2>Comparison of Search Methods</h2>
<p>

The simplest approach to lattice searching is a simple
box search on ranges in unit cell a, b and c
and possibly on &#945;, &#946;, and &#947;, as for example in the
&quot;cell dimensions&quot; option in the RCSB advanced search at
http://www.rcsb.org/pdb/search/advSearch.do for the Protein
Data Bank (Berman et al., 2000). In the following examples, we
will call that type of search &quot;Range&quot;. For the reasons discussed
above, such simple searches can fail to find unit cells representing
similar lattices but with very different edges that actually
represent similar lattices. Such searches are best characterized
as cell searches, rather than as lattice searches.
<P>
Searching on primitive reduced cells greatly improves the
reliability of a search, as for example in (Ramraj et al., 2011) at
http://www.strubi.ox.ac.uk/nearest-cell/nearest-cell.cgi, which
uses a metric based on the reduced cell and all permutations of
axes.While an improvement over simple range searches as discussed
above, such searches can also miss similar lattices if the
number of alternate lattice presentations considered is not complete.
One way to reduce such gaps in searches is to use only
parameters that do not depend on the choice of reduced presentation.
The (Andrews et al., 1980) approach using 7 parameters
(three reduced cell edges, three reduced reciprocal cell edges
and the volume), &quot;V7&quot;, helps, but has difficulty distinguishing
cells with angles near 90 degrees. The NCDist approach used
here, derived from (Andrews & Bernstein, 2012) both fills in
the gaps and handles angles near to 90 degrees.
<P>
Consider, for example, the unit cells of phospholipase A<sub>2</sub> discussed
by (Le Trong & Stenkamp, 2007). They present three
alternate cells from three different PDB entries that are actually
for the same structure:<br/>
[57.98, 57.98, 57.98, 92.02, 92.02, 92.02] from entry 1FE5
(Singh et al., 2001) in space group R32,<br/>
[80.36, 80.36, 99.44, 90, 90, 120] from entry 1U4J (Singh et al.,
2005) in space group R3 and<br/>
[80.949, 80.572, 57.098, 90.0, 90.35, 90.0] from entry 1G2X
(Singh et al., 2004) in space group C2. No simple Range
search can bring these three cells together. For example, if
we use the PDB advanced cell dimensions search around the
cell from IU4J with edge ranges of +-3 &#8491;ngstroms and angle
ranges of +-1 degree, we get 28 hits: 1CG5, 1CNV, 1FW2,
1G0Z, 1GS7, 1GS8, 1HAU, 1ILD, 1ILZ, 1IM0, 1LR0, 1NDT,
1OE1, 1OE2, 1OE3, 1QD5, 1U4J, 2BM3, 2BO0, 2H8A, 2HZ5,
2OHG, 2REW, 2WCE, 3I06, 3KKU, 3Q98, 3RP2, of which
only three actually have cells close to the target using the linearized
NCDist metric : 2WCE at 2.96 &#8491;ngstroms, 1G0Z at 0
&#8491;ngstroms, and 1U4J, the target itself. The remaining cells are,
as we will see, rejected under the Nearest-Cell and the V7 metric.
The simple Range searches are not appropriate to this problem.
<P>
Table 2 shows partial results from a lattice search using
Nearest-Cell, and a V7 search using SAUC and a NCDist search
using SAUC. We have restricted the searches to NCDist distances
&#8804; 3.5 &#8491;ngstroms. The Nearest-Cell metric appears to
be in &#8491;<sup>2</sup>. The column with the square root of the Nearest-
Cell metric facilitates comparison with the linearized SAUC V7
and NCDist metrics. The searches showed consistent behavior:
The three cells noted by (Le Trong & Stenkamp, 2007)
are found in the same relative positions by all three searches.
All cells found by Nearest-Cell are also found by both V7 and
NCDist. Of the 42 structures found by all three metrics within
3.5 &#8491;ngstroms under the NCDist metric, four (1G0Z, 1G2X,
1DPY and 1FE5) are E.C. class 3.1.1.4 Phospholipase A2 structures,
and three (1PKR, 1SGC and 1VRI) are other hydrolases
(E. C. classes 3.4.21.7, 3.4.21.80, and 3.4.19.2, respectively)
However, ten cells found by V7 and NCDist were not
found by Nearest-Cell (2OSN, 2CMP, 3MIJ, 2SGA, 2YZU,
3SGA, 4SGA, 5SGA, 1CDC and 2CVK). Of those ten, one
(2OSN) is an E.C class 3.1.1.4 Phospholipase A2 structure
and four (2SGA, 3SGA, 4SGA and 5SGA are hydrolases,
specifically E.C. class 3.4.21.80 Proteinase A. Two of the ten
(2YZU and 2CVK) are thioredoxin, for which the ProMOL
(Craig et al., submitted) motif finder shows significant active
site homologies to multiple hydorolase motifs (2YZU has site
homologies to 132L, 135L and 1LZ1 in E.C. class 3.2.1.17 and
to 4HOH in E.C. class 3.1.27.3, 2CVK to 1AMY in E.C. class
3.2.1.1, to 1BF2 in E.C. class 3.2.1.68, to 1EYI in class 3.2.3.11,
etc.). For 1CDC, a &quot;metastable structure of CD2&quot;, proMOL
shows an active site homology to 1ALK of E.C. class 3.1.3.1,
another hydrolase.
<P>
The significant gaps in the Nearest-Cell search do not appear
to be a problem of the distance for the Nearest-Cell search having
been cut off at a too-small value. For the common hits
between the square root of the Nearest-Cell metric and the linearized
NCDist metric, a linear fit is excellent, with R<sup>2</sup> = 0.89
and no points are very far from the line. The agreement of the
linearized V7 to the other two metrics is much noisier because
of loss of sensitivity of the V7 metric for angles near 90 degrees
and the inherent difficulty the V7 metric has in discriminating
between the + + + and - - - parts of the Niggli cone. For
example, 1GUT (Sch&#252;ttelkopf et al., 2002) is at distances 1.2
and 3.7 from 1UJ4 in the Nearest-Cell and linearized NCDist
metrics, respectively, but only 0.1 in the V7 metric. The 1GUT
cell is<br/>
[78.961, 82.328, 57.031, 90.00, 93.44, 90.00] in C 1 2 1, Z=24,
with a primitive cell<br/>
[57.031, 57.0367, 57.0367, 92.3918, 92.3804, 92.3804]
which corresponds to a G<sup>6</sup> vector<br/>
[3252.53, 3253.18, 3253.18,-271.53,-270.208,-270.208]
and a linearized V7 vector<br/>
[52.8004, 52.8057, 52.8057, 52.7101, 52.7101, 52.7053, 52.7569].
The 1U4J cell is<br/>
[80.36, 80.36, 99.44, 90, 90, 120] in R3, Z=18,
with a primitive cell<br/>
[57.02, 57.02, 57.02, 89.605, 89.605, 89.605]
which corresponds to a G<sup>6</sup> vector<br/>
[3251.28, 3251.28, 3251.28, 44.8265, 44.8265, 44.8265]
and a linearized V7 vector<br/>
[52.7902, 52.7902, 52.7902, 52.7878, 52.7878, 52.7878, 52.789]
This is almost identical to the 1GUT V7 vector, even though the
corresponding primitive cells and G<sup>6</sup> cells differ significantly.

<p>
<a name="Tables"></a>
<h2>Tables</h2>
<p>
Table 1
<p>

Table 2

<p>
<a name="References"></a>
<h2>References</h2>
<p>

Allen, F. H., Kennard, O., Motherwell, W. D. S., Town, W. G. & Watson, D. G. (1973). Journal of Chemical Documentation, 13(3), 119 - 123.
<br />
Andrews, L. (2001). C/C++ Users Journal, 19, 40 - 49. http://sf.net/projects/neartree.
<br />
Andrews, L. C. & Bernstein, H. J. (2012). arXiv preprint arXiv:1203.5146.
<br />
Andrews, L. C., Bernstein, H. J. & Pelletier, G. A. (1980). Acta Crystallogr. A36, 248 - 252.
<br />
Berman, H. M., Westbrook, J., Feng, Z., Gilliland, G., Bhat, T. N., Weissig, H., Shindyalov, I. N. & Bourne, P. E. (2000). Nucleic Acids Res. 28,
235 - 242.
<br />
Bernstein, F. C., Koetzle, T. F., Williams, G. J. B., Meyer, Jr., E. F., Brice, M. D., Rodgers, J. R., Kennard, O., Shimanouchi, T. & Tasumi, M.
(1977). J. Mol. Biol. 112, 535 - 542.
<br />
Burchard, U. (1998). The Mineralogical Record, 29(6), 517 - 583.
<br />
Craig, P. A., Hanson, B., Westin, C., Rosa, M., Bernstein, H. J., Grier, A., Osipovitch, M., MacDonald, M., Dodge, G., Boli, P. M., Corwin, C. W.
& Kessler, H. (submitted). B. M. C. Bioinformatics.
<br />
Donnay, J. D. H. (1943). The American Minerologist, 28, 313.
<br />
Johnson, G. G. (2013). Private Communication.
<br />
Knuth, D. E. (1973). Sorting and Searching (The Art of Computer Programming volume 3). Addison Wesley, Reading, MA.
<br />
Le Trong, I. & Stenkamp, R. E. (2007). Acta Crystallogr. D63, 548 - 549.
<br />
Mighell, A. D. (2001). Journal of Research - National Institute of Standards and Technology, 106(6), 983 - 996.
<br />
Mighell, A. D. (2002). Journal of Research - National Institute of Standards and Technology, 107(5), 425 - 430.
<br />
NIH/EPA (1980). User''`s Manual NIH-EPA Chemical Information System, chap. User's Guide to CRYST The X-Ray Crystallographic Search
System. National Institutes of Health, Environmental Protections Agency.
<br />
Pearson, W. B. (1958). Handbook of Lattice Spacings and Structures of Metals and Alloys. International Series of Monographs on Metal and
Physics and Physical Metallurgy, G. V. Raynor (ed.). Pergamon Press.
<br />
Ramraj, V., Esnouf, R. & Diprose, J. (2011). Nearest-Cell A fast and easy tool for locating crystal matches in the PDB. Tech. rep. Division of
Structural Biology, University of Oxford. http://www.strubi.ox.ac.uk/nearest-cell/nearest-cell.cgi.
<br />
Sch&#252;ttelkopf, A. W., Harrison, J. A., Boxer, D. H. & Hunter, W. N. (2002). Journal of Biological Chemistry, 277(17), 15013 - 15020.
<br />
Singh, G., Gourinath, S., Saravanan, K., Sharma, S., Bhanumathi, S., Betzel, C., Srinivasan, A. & Singh, T. P. (2004). Acta Crystallographica
Section F: Structural Biology and Crystallization Communications, 61(1), 8 - 13.
<br />
Singh, G., Gourinath, S., Sarvanan, K., Sharma, S., Bhanumathi, S., Betzel, C., Yadav, S., Srinivasan, A. & Singh, T. P. (2005). J. Struct. Biol.
149(3), 264 - 272.
<br />
Singh, G., Gourinath, S., Sharma, S., Paramasivam, M., Srinivasan, A. & Singh, T. P. (2001). Journal of molecular biology, 307(4), 1049 - 1059.
<br />
Thomas, I. R., Bruno, I. J., Cole, J. C., Macrae, C. F., Pidcock, E. & Wood, P. A. (2010). J. Appl. Crystallogr. 43(2), 362 - 366.
<br />
Toby, B. (1994). Private Communication.
<br />
Wyckoff, R. W. G. (1931). The structure of crystals. No. 19. The Chemical Catalog Company. inc.

<p>
<hr />
<center>
| <a href="#Search">Search</a>
| <a href="#Source">Source</a>
| <a href="#Introduction">Introduction</a>
| <a href="#History">History</a>
| <a href="#Background">Background</a>
| <a href="#Implementation1">Implementation - Distance</a>
| <a href="#Implementation2">Implementation - Searching</a>
| <a href="#Comparison">Comparison of Search Methods</a>
| <a href="#Tables">Tables</a>
| <a href="#References">References</a>
|
</center>

<hr>
Updated 24 June  2013.
</font>
</body>
</html>'

