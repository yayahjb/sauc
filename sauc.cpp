/* sauc.cpp  -- Search of Alternate Unit Cells
 (C) Copyright Keith McGill 2013
 
   rev 27 Mar 2013 -- HJB
   rev 26 May 2013 -- LCA
   rev 27 May 2013 -- HJB
   rev 24 Jun 2013 -- HJB
   rev  6 Jul 2013 -- HJB
   rev 22 Apr 2014 -- HJB
   rev 20 Mar 2016 -- HJB
 
     *******************************************************
         You may redistribute this program under the terms
         of the GPL.
         
         Alternatively you may redistribute this functions
         and subroutines of this program as an API under the
         terms of the LGPL
     *******************************************************
 *************************** GPL NOTICES ******************************
 *                                                                    *
 * This program is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License as     *
 * published by the Free Software Foundation; either version 2 of     *
 * (the License, or (at your option) any later version.               *
 *                                                                    *
 * This program is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this program; if not, write to the Free Software        *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA           *
 * 02111-1307  USA                                                    *
 *                                                                    *
 **********************************************************************
 
 ************************* LGPL NOTICES *******************************
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU Lesser General Public         *
 * License as published by the Free Software Foundation; either       *
 * version 2.1 of the License, or (at your option) any later version. *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 * Lesser General Public License for more details.                    *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License along with this library; if not, write to the Free         *
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,    *
 * MA  02110-1301  USA                                                *
 *                                                                    *
 **********************************************************************
 
 */

//#include <stdio.h>
//#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <iostream>
#include <cstring>
#define USE_ARMADILLO_LIBRARY
#include "TNear.h"
#include "triple.h"
#include "rhrand.h"
#include "unitcell.h"
#include <fstream>
#include <string>
#include <sstream>
#include "V7.h"
#include "Cell.h"
#include "Reducer.h"
#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK
#include <armadillo>
#include <vector>

/* Forward Declaration */
void SphereResults( std::ostream& out,
                   const std::vector<unitcell>&  myvector,
                   const std::vector<size_t>&    myindices,
                   const std::vector<double>&    mydistances,
                   const unitcell&               unknownCell);

using namespace std;

CNearTree <unitcell> * cellTree[4] = {NULL,NULL,NULL,NULL};
CNearTree <unitcell>::iterator cellTree_itend[4];
string filenames[6];
string CSDfilenames[2];

const int NUM_COLUMNS = 9, NUM_ROWS = 2000000, NUM_DUMP = 2;
const int NUM_CSDDUMP = 1;
//const int NUM_COLUMNS = 9, NUM_ROWS = 10000, NUM_DUMP = 2;
//const int NUM_ROWS = 100;

string idArray[NUM_ROWS][1];
double cellDArray[NUM_ROWS][6];
string spaceArray[NUM_ROWS][1];
string entryArray[NUM_ROWS][8];
long entryhash[131072];
long save_entryhash[131072];
long entryhashlink[NUM_ROWS];
long entryhashvalue[NUM_ROWS];
long headhash[131072];
long save_headhash[131072];
long headhashlink[NUM_ROWS];
long headhashvalue[NUM_ROWS];
int zArray[NUM_ROWS][1];
int numentries;
int noCSD;
double probeArray[6];
arma::vec6 primredprobe;
string probelattice;
double avglen=1.;
size_t num_rows=1;
double crootvol;

int nearest = 0, numRow = 0;
int quitAlgorithm = 0, quitSimilar = 0, quitContinue = 0, endProgram = 0, choiceAlgorithm = 0, choiceSimilar = 0, choiceContinue = 0, goBack = 0,
priorAlgorithm = -1;
double result0 = 0, result1 = 0, result2 = 0, result3 = 0, result4 = 0, result5 = 0, resultSumTemp = 0, resultSum = 0, numRange = 0;
double numRangeA, numRangeB, numRangeC, numRangeAlpha, numRangeBeta, numRangeGamma, sphereRange;
string valueDump;
int sauc_batch_mode = 0;
int sauc_javascript = 0;

//*****************************************************************************
double convertToDouble(string value)
{
    double number;
    istringstream issvalue(value);
    if (!(issvalue >> number)) number = 0;
    return number;
}

//*****************************************************************************
int convertToInt(string zvalue)
{
	int number = 0;
    istringstream issvalue(zvalue);
    if (!(issvalue >> number)) number = 0;
    return number;
}

inline long hashvalue(string str) {
    unsigned int ii;
    unsigned long hash;
    const char * strasstr = str.c_str();
    hash = 0;
    for(ii=0;ii<str.length();ii++) {
        hash = ((hash << 4)| (hash>>28))^((unsigned long)(tolower(strasstr[ii]))-32);
    }
    return (long) (hash&0x10000);
}

vector<string> split(const string& str, char delim) {
    vector<string> values;
    const char * strasstr = str.c_str();
    unsigned int istart,iend;
    istart=0;
    while (istart < str.length()){
        for (iend=istart;iend<str.length();iend++){
            if (strasstr[iend] == delim) {
                values.push_back(str.substr(istart,iend-istart));
                istart = iend+1;
                break;
            }
        }
        if (iend < str.length()) continue;
        values.push_back(str.substr(istart,iend-istart));
        istart = iend+1;
        break;
    }
    return values;
}

//*****************************************************************************
void makeEntryDatabase(string filename)
{
    std::ifstream infile;
    string line,id,head,acc,cmpd,src,aut,res,exp;
    numentries = 0;
    long hash;
    long hashnext;
    long ii;
    
    for (ii=0; ii < 131072; ii++) entryhash[ii] = headhash[ii] = -1;
    for (ii=0; ii < NUM_ROWS; ii++) entryhashlink[ii] = headhashlink[ii] = -1;
    
    infile.open(filename.c_str());
    getline(infile,line);
    getline(infile,line);
    while (getline(infile,line)) {
        line += "\t\t\t\t\t\t\t\t";
        vector<string> values = split(line,'\t');
        id = values[0];
        head = values[1];
        acc = values[2];
        cmpd = values[3];
        src = values[4];
        aut = values[5];
        res = values[6];
        exp = values[7];
        /* if (numentries < 20) {
            for (ii=0; ii < values.size(); ii++)
            std::cout << ii << " " << values[ii] << std::endl;
            std::cout << id << head << acc << cmpd << src << aut << res << exp << std::endl;
        } */
        entryArray[numentries][0] = id;
        entryArray[numentries][1] = head;
        entryArray[numentries][2] = acc;
        entryArray[numentries][3] = cmpd;
        entryArray[numentries][4] = src;
        entryArray[numentries][5] = aut;
        entryArray[numentries][6] = res;
        entryArray[numentries][7] = exp;
        hash = hashvalue(id);
        entryhashvalue[numentries] = hash;
        hashnext = entryhash[hash];
        entryhash[hash] = numentries;
        entryhashlink[numentries] = hashnext;
        hash = hashvalue(head);
        headhashvalue[numentries] = hash;
        hashnext = headhash[hash];
        headhash[hash] = numentries;
        headhashlink[numentries] = hashnext;
        numentries++;
    }

}

long findEntryDatabase(string id) {
    long hash=hashvalue(id);
    long hashnext;
    
    hashnext = entryhash[hash];
    while (hashnext >= 0) {
        if (!strcasecmp(id.c_str(),entryArray[hashnext][0].c_str())) {
            return hashnext;
        }
        hashnext = entryhashlink[hashnext];
    }
    return -1L;
}

long findHeadDatabase(string id) {
    long hash=hashvalue(id);
    long hashnext;
    
    hashnext = headhash[hash];
    while (hashnext >= 0) {
        if (!strcasecmp(id.c_str(),entryArray[hashnext][1].c_str())) {
            return hashnext;
        }
        hashnext = headhashlink[hashnext];
    }
    return hashnext;
}


//*****************************************************************************
void makeDatabase(string filename)
{
    // DB ID,Angle Alpha,Angle Beta,Angle Gamma,Length A,Length B,Length C,Space Group,Z-number
	string format;
	int i;
	std::ifstream infile;
	infile.open(filename.c_str());
	for (int i = 0; i < NUM_DUMP; i++)
	{
		getline (infile, valueDump, 'r');
		//std::cout << valueDump << std::endl;
	}
	avglen = 0.;
	for (i = 0; i < NUM_ROWS; i++)
	{
	string value;
        
        //if (i%100 == 0) std::cout << i << std::endl;
        if (!infile.good()) break;
		getline (infile, value, ',');
        if (value.length()<3) break;
		//std::cout << value << std::endl;
		idArray[i][0] = string(value, 2, value.length()-3);
		//std::cout << idArray[i][0] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, value, ',');
		//std::cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//std::cout << value << std::endl;
		cellDArray[i][3] = convertToDouble(value);
		//std::cout << cellDArray[i][3] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, value, ',');
		//std::cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//std::cout << value << std::endl;
		cellDArray[i][4] = convertToDouble(value);
		//std::cout << cellDArray[i][4] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, value, ',');
		//std::cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//std::cout << value << std::endl;
		cellDArray[i][5] = convertToDouble(value);
		//std::cout << cellDArray[i][5] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, value, ',');
		//std::cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//std::cout << value << std::endl;
		cellDArray[i][0] = convertToDouble(value);
        avglen += cellDArray[i][0];
		//std::cout << cellDArray[i][0] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, value, ',');
		//std::cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//std::cout << value << std::endl;
		cellDArray[i][1] = convertToDouble(value);
        avglen += cellDArray[i][1];
		//std::cout << cellDArray[i][1] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, value, ',');
		//std::cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//std::cout << value << std::endl;
		cellDArray[i][2] = convertToDouble(value);
        avglen += cellDArray[i][2];
		//std::cout << cellDArray[i][2] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, value, ',');
		//std::cout << value << std::endl;
		spaceArray[i][0] = string(value, 1, value.length()-2);
		//std::cout << spaceArray[i][0] << std::endl;
        
        if (!infile.good()) break;
		getline (infile, valueDump, '"');
        if (!infile.good()) break;
		getline (infile, value, '"');
		//std::cout << value << std::endl;
		value = string(value, 0, value.length());
		//std::cout << value << std::endl;
		zArray[i][0] = convertToInt(value);
		//std::cout << zArray[i][0] << std::endl;
        
		//std::cout << "-----------------------------------------------" << std::endl;
	}
    if (i > 0)
        avglen /= double(3*i);
    num_rows = i;
    std::cout << "avglen from PDB: " << avglen << std::endl;
	infile.close();
}

/* This is a tokenizer for csv strings that unconditionally recognizes the character c
   to separate fields, strips leading and trailing blanks and control characters
   from tokens and also strips one layer of single or double quote marks. */

void string_tokens(const std::string& str, std::vector<std::string>& tokens, const char c) {
    size_t ic, iq, ieq, istart, iend;
    char qc;
    istart = 0;
    iend = 0;
    tokens.clear();
    
    //std::cout << "tokenizing: " << str << std::endl;
    //std::cout << "using separator: "<< c << std::endl;
    
    ic = 0;
    while (ic < str.length()+1) {
        if (ic == str.length() || str[ic] == c) {
            iend = ic;
            ic = iend+1;
            // std::cout << "changed ic to :"<< ic << std::endl;
            if (iend > istart) {
                qc = '\0';
                for (iq = istart; iq < iend; iq++) {
                    /* scan for quoted string */
                    if (str[iq]=='"' || str[iq]=='\'') {
                        qc = str[iq];
                        for (ieq = iend-1; ieq > iq; ieq--) {
                            if (isblank(str[ieq])|| str[ieq] < 32) continue;
                            if (str[ieq]==qc) {
                                istart = iq+1;
                                iend = ieq;
                                break;
                            } else {
                                istart = iq+1;
                                iend = ieq+1;
                                break;
                            }
                        }
                        break;
                    /* blank-strip string */
                    } else if (!isblank(str[iq]) && str[iq] >= 32 ){
                        istart = iq;
                        for (ieq = iend-1; ieq >= iq; ieq--) {
                            if (isblank(str[ieq]) || str[ieq] < 32 ) continue;
                            iend = ieq+1;
                            // std::cout  << "token from " << istart << " to " << iend << std::endl;
                            break;
                        }
                        break;
                    }
                }
                if (iend > istart && istart < str.length()) {
                    tokens.push_back(str.substr(istart,iend-istart));
                } else {
                    tokens.push_back(std::string(""));
                }
            } else {
                tokens.push_back(std::string(""));
            }
            istart = ic;
            iend = 0;
        } else {
            ic++;
        }
    }
}



//*****************************************************************************
int makeCSDDatabase(string filename, string ckpfilename)
{
    // refcode, a, b, c, alpha, beta, gamma, spacegroup, centring, r_a, r_b, r_c, r_alpha, r_beta, r_gamma
    
    int i, ii, save_rows, save_entries;
    std::ifstream infile;
    std::ofstream serialout;
    std::ifstream serialin;
    std::string line;
    std::vector<std::string> tokens;
    long hash;
    long hashnext;
    bool gotckp = false;

    infile.open(filename.c_str());
    if (infile.fail()) return -1;


    serialin.open(ckpfilename.c_str(),std::ios::in);
    save_rows = num_rows;
    save_entries = numentries;
    for (i=0; i < 131072; i++) {
        save_entryhash[i] = entryhash[i];
        save_headhash[i] = headhash[i];
    }
    
    i = 0;
    while (true) {
        string token;
        if (serialin.is_open()) {
            std::cout << "Reading CSD database " << ckpfilename << std::endl;
            serialin >> token;
            if (!serialin.good()){
                std::cout << ckpfilename << "failed to read initial token" << std::endl;
                break;
            } else if (token != string("CSDdatabase:")) {
                std::cout << ckpfilename
                << " badly formatted, no 'CSDdatabase:' token, instead have "
                << token
                << std::endl;
                break;
            }
            while (serialin.good()) {
                int row;
                int entry;
                string refcode;
                double cell[6];
                string space;
                string ochar;
                string cchar;
                row = -9999999;
                serialin >> ochar >> row >> entry >> refcode
                    >> cell[0] >> cell[1] >> cell[2] >> cell[3] >> cell[4] >> cell[5]
                    >> space >> cchar;
                /* row -1 is terminator */
                if (row == -1) {
                    gotckp = true;
                    break;
                }
                if (!serialin.good()
                    || ochar !=string("{") or cchar != string("}")
                    || refcode.length() < 6
                    || cell[0] < 0.1 || cell[1] < 0.1 || cell[2] < 0.1
                    || cell[3] < 5. || cell[4] < 5. || cell[5] < 5.
                    || cell[3] > 175. || cell[4] > 175. || cell [5] > 175.
                    || row != num_rows+i || entry != numentries+i ) {
                    std::cout << ckpfilename  << " badly formatted row " << i << std::endl;
                    break;
                    
                }
                for (ii=0; ii<6; ii++) cellDArray[num_rows+i][ii] = cell[ii];
                idArray[num_rows+i][0] = refcode;
                entryArray[numentries+i][0] = refcode;
                entryArray[numentries+i][1] = refcode.substr(0,6);
                entryArray[numentries+i][2] = "";
                entryArray[numentries+i][3] = "";
                entryArray[numentries+i][4] = "";
                entryArray[numentries+i][5] = "";
                entryArray[numentries+i][6] = "";
                entryArray[numentries+i][7] = "";
                hash = hashvalue(entryArray[numentries+i][0]);
                entryhashvalue[numentries+i] = hash;
                hashnext = entryhash[hash];
                entryhash[hash] = numentries+i;
                entryhashlink[numentries+i] = hashnext;
                hash = hashvalue(entryArray[numentries+i][1]);
                headhashvalue[numentries+i] = hash;
                hashnext = headhash[hash];
                headhash[hash] = numentries+i;
                headhashlink[numentries+i] = hashnext;
                spaceArray[num_rows+i][0] = space;
                zArray[num_rows+i][0] = -1;
                i++;
                
            }
        } else {
            break;
        }
        break;
    }
    if (serialin.is_open()) serialin.close();
    if (gotckp) {
        numentries += i;
        num_rows += i;
        if (infile.is_open()) infile.close();
        return 0;
    }
    
    num_rows = save_rows;
    numentries = save_entries;
    for (i=0; i < 131072; i++) {
        entryhash[i] = save_entryhash[i];
        headhash[i] = save_headhash[i];
    }

    serialout.open(ckpfilename.c_str(),std::ios::out|std::ios::trunc);
    if (serialout.is_open()) {
        serialout <<  "CSDdatabase:" << std::endl;
    }
    
    for (int i = 0; i < NUM_CSDDUMP; i++)
    {
        std::getline (infile, line);
        //std::cout << valueDump << std::endl;
    }
    avglen *= double(3*num_rows);
    for (i = num_rows; i < NUM_ROWS; i++)
    {
        string value;
        int istart;
        int goodcell;
        
        if (infile.eof()) break;
        
        std::getline(infile,line);
        string_tokens(line, tokens, ',');
        istart = 0;
        
        
        if (!infile.good() || tokens.size() < 8) {
            std::cout << "unable to process " << line << std::endl;
            i --;
            continue;
        }
        //if (i%10000 == 0 || tokens.size() < 15) {
        //    std::cout << i << " tokens: " << tokens.size() << std::endl;
        //}

        
        // for (ii = 0; ii < tokens.size(); ii++) std::cout << tokens[ii] << "|";
        
        // std::cout << std::endl;
        
        if (tokens[0].length()<6) {
            std::cout << "unable to process, bad CSD refcode " << line << std::endl;
            i --;
            continue;
        }
        
        // std::cout << idArray[i][0] << std::endl;
        
        // Read a, b, c, alpha, beta, gamma
        
        goodcell = 1;
        
        for (ii=0; ii < 6; ii++){
            if (!infile.good()) break;
            value = tokens[ii+1];
            cellDArray[i][ii] = convertToDouble(value);
            if (ii < 3) {
                if ( cellDArray[i][ii] < 0.1) {
                    std::cout << "unable to process "<< tokens[0] <<", bad cell edge  "<< value << std::endl;
                    goodcell = 0;
                    break;
                }
                avglen += cellDArray[i][ii];
            } else {
                if ( cellDArray[i][ii] < 5. || cellDArray[i][ii] > 175.) {
                    std::cout << "unable to process "<< tokens[0] <<", bad cell angle  "<< value << std::endl;
                    goodcell = 0;
                    break;
                }
                
            }
            // std::cout << cellDArray[i][ii] << std::endl;
        }
        
        if (!goodcell) {
            i--;
            continue;
        }

        if (serialout.is_open()) {
            serialout << "{ " << i << " "
            << numentries << " "
            << tokens[0] << " "
            << cellDArray[i][0] << " "
            << cellDArray[i][1] << " "
            << cellDArray[i][2] << " "
            << cellDArray[i][3] << " "
            << cellDArray[i][4] << " "
            << cellDArray[i][5] << " "
            << tokens[7]
            << " }" << std::endl;
        }
        
        idArray[i][0] = tokens[0];
        entryArray[numentries][0] = tokens[0];
        entryArray[numentries][1] = tokens[0].substr(0,6);
        entryArray[numentries][2] = "";
        entryArray[numentries][3] = "";
        entryArray[numentries][4] = "";
        entryArray[numentries][5] = "";
        entryArray[numentries][6] = "";
        entryArray[numentries][7] = "";
        hash = hashvalue(entryArray[numentries][0]);
        entryhashvalue[numentries] = hash;
        hashnext = entryhash[hash];
        entryhash[hash] = numentries;
        entryhashlink[numentries] = hashnext;
        hash = hashvalue(entryArray[numentries][1]);
        headhashvalue[numentries] = hash;
        hashnext = headhash[hash];
        headhash[hash] = numentries;
        headhashlink[numentries] = hashnext;
        numentries++;

        
        if (!infile.good()) break;
        spaceArray[i][0] = tokens[7];
        //std::cout << spaceArray[i][0] << std::endl;
        
        zArray[i][0] = -1;
        
        //std::cout << "-----------------------------------------------" << std::endl;
    }
    if (i > 0)
        avglen /= double(3*i);
    
    std::cout << "avglen from PDB + CSD: " << avglen << std::endl;
    num_rows = i;
    infile.close();
    if (serialout.is_open()) {
        serialout << "{ " << -1 << " "
        << -1 << " "
        << "------" << " "
        << 1. << " "
        << 1. << " "
        << 1. << " "
        << 90. << " "
        << 90. << " "
        << 90. << " "
        << "P1"
        << " }" << std::endl;
        serialout.close();
    }

    return 0;

}

//*****************************************************************************
bool makeprimredprobe( void )
{
    string latsym;
    char clatsym;
    arma::vec6 v6cell;
    arma::vec6 redprimcell;
    arma::mat66 mc;
    arma::mat66 m;
    arma::vec6 primcell;
    arma::vec6 recipcell;
    arma::vec6 reducedBase;
    Cell rawcell(probeArray[0],probeArray[1],probeArray[2],
                 probeArray[3],probeArray[4],probeArray[5]);
    int ii;
    bool ret;
    if (probelattice.size()< 1) {
        latsym = "P";
    } else {
        latsym = probelattice.substr(0,1);
    }
    clatsym = latsym[0];
    switch (clatsym) {
        case 'P':
        case 'p':
        case 'A':
        case 'a':
        case 'B':
        case 'b':
        case 'C':
        case 'c':
        case 'I':
        case 'i':
        case 'F':
        case 'f':
        case 'R':
        case 'r':
        case 'H':
        case 'h':
            mc = rawcell.LatSymMat66(latsym);
            primcell = mc*(rawcell.Cell2V6());
            break;
        case 'V':
        case 'v':
            for (ii=0; ii< 6; ii++) {
                primcell[ii] = probeArray[ii];
            }
            break;
        default:
            cerr << "Unrecognized lattice symbol "<< probelattice<<" treated as P" << std::endl;
            latsym = "P";
            mc = rawcell.LatSymMat66(latsym);
            primcell = mc*(rawcell.Cell2V6());
            break;
    }
    ret = Reducer::Reduce(primcell,m,redprimcell,0.0);
    primredprobe = Cell(redprimcell).CellWithDegrees();
    std::cout << "Primitive Reduced Probe Cell: " <<
    primredprobe[0]<<" "<<
    primredprobe[1]<<" "<<
    primredprobe[2]<<" "<<
    primredprobe[3]<<" "<<
    primredprobe[4]<<" "<<
    primredprobe[5] << std::endl;
    std::cout << "Volume :" << Cell(redprimcell).Volume() << std::endl;
    crootvol = pow(Cell(redprimcell).Volume(),1./3.);
    Reducer::Reduce((Cell(redprimcell).Inverse()).Cell2V6(),m,reducedBase,0.0);
    recipcell = (Cell(redprimcell).Inverse()).CellWithDegrees();
    
    std::cout << "Reciprocal of Primitive Probe Cell: " <<
    recipcell[0]<<" "<<
    recipcell[1]<<" "<<
    recipcell[2]<<" "<<
    recipcell[3]<<" "<<
    recipcell[4]<<" "<<
    recipcell[5]<< std::endl;
    std::cout << "Volume of Reciprocal Cell: " <<
    (Cell(redprimcell).Inverse()).Volume() << std::endl;
    std::cout << "V7 linearized and scaled: "
    << primredprobe[0]*std::sqrt(6./7.)<<" "
    << primredprobe[1]*std::sqrt(6./7.)<<" "
    << primredprobe[2]*std::sqrt(6./7.)<<" "
    << std::sqrt(1./reducedBase[0])*std::sqrt(6./7.)<<" "
    << std::sqrt(1./reducedBase[1])*std::sqrt(6./7.)<<" "
    << std::sqrt(1./reducedBase[2])*std::sqrt(6./7.)<<" "
    << " "<<
    crootvol*std::sqrt(6./7.)<<std::endl;
    if (latsym[0] == 'V' || latsym[0] == 'v') {
        std::cout << "raw G6 vector: "
        << primcell[0]<<" "
        << primcell[1]<<" "
        << primcell[2]<<" "
        << primcell[3]<<" "
        << primcell[4]<<" "
        << primcell[5]<<std::endl;
    } else {
        std::cout << "raw G6 vector: "
        << primredprobe[0]*primredprobe[0]<<" "
        << primredprobe[1]*primredprobe[1]<<" "
        << primredprobe[2]*primredprobe[2]<<" "
        << 2.*primredprobe[1]*primredprobe[2]*cos(primredprobe[3]*std::atan(1.0)/45.)<<" "
        << 2.*primredprobe[0]*primredprobe[2]*cos(primredprobe[4]*std::atan(1.0)/45.)<<" "
        << 2.*primredprobe[0]*primredprobe[1]*cos(primredprobe[5]*std::atan(1.0)/45.)<<std::endl;
    }
	std::cout << std::endl;
    return ret;
}

#define putoutst(serialout,value) \
if (value != ULONG_MAX) { \
(serialout) << (value); \
} else { \
(serialout) << "-1"; \
} \


//*****************************************************************************
void buildNearTree( void )
{
    arma::mat66 m;
    arma::mat66 mc;
    arma::vec6 primcell;
    arma::vec6 redprimcell;
    arma::vec6 searchcell;
    std::ofstream serialout;
    std::ifstream serialin;
    size_t si;
    size_t sv;
    double cell[6];
    double row;
    unitcell ucell;
    std::vector<long> * DelayedIndices; // objects queued for insertion, possibly in random order
    std::vector<unitcell>  * ObjectStore;    // all inserted objects go here
    std::vector<size_t>    * ObjectCollide;  // overflow chain of colliding objects
    size_t            DeepestDepth;     // maximum depth of the tree
    std::vector< CNearTree<unitcell>::NearTreeNode<unitcell> * >
    * NearTreeNodes;  // vector of pointers to nodes to build the tree
    CNearTree<unitcell>::NearTreeNode<unitcell>
    * BaseNode;       // the tree's data is stored down
    // this node in m_NearTreeNodes
    long              Flags;            // flags for operational control (mainly for testing)
    double            DiamEstimate;     // estimated diameter
    double            SumSpacings;      // sum of spacings at time of insertion
    double            SumSpacingsSq;    // sum of squares of spacings at time of insertion
    double            DimEstimate;      // estimated dimension
    double            DimEstimateEsd;   // estimated dimension estimated standard deviation
#ifdef CNEARTREE_INSTRUMENTED
    size_t            NodeVisits;       // number of node visits
#endif
    CNearTree<unitcell>::NearTreeNode<unitcell> * curntn;
        int objno = 0;
    bool gotckp = false;
    
    serialin.open(filenames[choiceAlgorithm].c_str(),std::ios::in);
    objno = 0;
    while (true) {
        string token;
        int Algorithm = 0;
        if (serialin.is_open()) {
            std::cout << "Reading database " << filenames[choiceAlgorithm] << std::endl;
            serialin >> token; if (!serialin.good() ||
                                   (token != string("Algorithm:")
                                    && token != string("Metric:"))) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'Metric' or 'Algorithm:' token" << std::endl;
                break;
            }
            serialin >> Algorithm;
            if (!serialin.good() || algorithm != choiceAlgorithm) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, algorithm value wrong" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DelayedIndices:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'DelayedIndices:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no DelayedIndices '{' token" << std::endl;
                break;
            }
            DelayedIndices = new std::vector<long>();
            while (serialin.good()) {
                serialin >> token;
                if (!serialin.good()) break;
                if (token == string("}")) break;
                DelayedIndices->push_back(atol(token.c_str()));
            }
            if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted DelayedIndices" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("ObjectStore:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'ObjectStore:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no ObjectStore '{' token" << std::endl;
                break;
            }
            ObjectStore = new std::vector<unitcell>();
            while (serialin.good()) {
                double cell[6];
                double row;
                serialin >> token;
                if (token == string("}")) break;
                if (!serialin.good() || token != string("{")) {
                    std::cout << filenames[choiceAlgorithm] << " badly formatted, missing ObjectStore '{' token" << std::endl;
                    break;
                }
                serialin >> cell[0] >> cell[1] >> cell[2] >> cell[3] >> cell[4] >> cell[5] >> row;
                objno++;
                if (!serialin.good()) {
                    std::cout << filenames[choiceAlgorithm] << " badly formatted, bad object: " << objno  << std::endl;
                    break;
                }
                ObjectStore->push_back(unitcell(cell,row));
                serialin >> token; if (!serialin.good() || token != string("}")) {
                    std::cout << filenames[choiceAlgorithm] << " badly formatted, missing ObjectStore '}' token" << std::endl;
                    break;
                }
                
            }
            if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted ObjectStore" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("ObjectCollide:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'ObjectCollide:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no ObjectCollide '{' token" << std::endl;
                break;
            }
            ObjectCollide = new std::vector<size_t>();
            while (serialin.good()) {
                long longval;
                serialin >> token;
                if (!serialin.good()) break;
                if (token == string("}")) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    ObjectCollide->push_back(size_t(longval));
                } else {
                    ObjectCollide->push_back(ULONG_MAX);
                }
            }
            if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted ObjectCollide" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DeepestDepth:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'DeepestDepth:' token" << std::endl;
                break;
            }
            
            serialin >> DeepestDepth; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted DeepestDepth" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("NearTreeNodes:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'NearTreeNodes:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no NearTreeNodes '{' token" << std::endl;
                break;
            }
            NearTreeNodes = new std::vector< CNearTree<unitcell>::NearTreeNode<unitcell> * >;
            BaseNode = NULL;
            while (serialin.good()) {
                long longval;
                size_t            ptLeft;            // index of left m_Object (of type TNode) stored in this node
                size_t            ptRight;           // index of right m_Object (of type TNode) stored in this node
                double            dMaxLeft;          // longest distance from the left m_Object to
                // anything below it in the tree
                double            dMaxRight;         // longest distance from the right m_Object to
                // anything below it in the tree
                size_t            pLeftBranch;       // tree descending from the left m_Object
                size_t            pRightBranch;      // tree descending from the right m_Object
                size_t            iTreeSize;         // size of this node tree
#ifdef CNEARTREE_INSTRUMENTED
                size_t            iHeight;           // height of the tree
                size_t            imultLeft;         // multiplicity at the left m_Object position
                size_t            imultRight;        // multiplicity at the right m_Object position
#endif
                serialin >> token;
                if (!serialin.good()) break;
                if (token == string("}")) break;
                
                if (token != string("{")) {
                    std::cout << filenames[choiceAlgorithm] << " badly formatted, no NearTreeNode '{' token " << token << std::endl;
                    break;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    ptLeft = size_t(longval);
                } else {
                    ptLeft = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    ptRight = size_t(longval);
                } else {
                    ptRight = ULONG_MAX;
                }
                
                serialin >> dMaxLeft >> dMaxRight;
                if (!serialin.good()) break;
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    pLeftBranch = size_t(longval);
                } else {
                    pLeftBranch = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    pRightBranch = size_t(longval);
                } else {
                    pRightBranch = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    iTreeSize = size_t(longval);
                } else {
                    iTreeSize = ULONG_MAX;
                }
                
#ifdef CNEARTREE_INSTRUMENTED
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    iHeight = size_t(longval);
                } else {
                    iHeight = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    imultLeft = size_t(longval);
                } else {
                    imultLeft = ULONG_MAX;
                }
                
                serialin >> token;
                if (!serialin.good()) break;
                longval = atol(token.c_str());
                if (longval != -1) {
                    imultRight = size_t(longval);
                } else {
                    imultRight = ULONG_MAX;
                }
#endif
                serialin >> token; if (!serialin.good() || token != string("}")) {
                    std::cout << filenames[choiceAlgorithm] << " badly formatted, no NearTreeNode '}' token: " << token <<std::endl;
                    break;
                }
                
                if (!BaseNode) {
                    BaseNode = new CNearTree<unitcell>::NearTreeNode<unitcell>
                    (ptLeft, ptRight, dMaxLeft, dMaxRight,pLeftBranch, pRightBranch,iTreeSize
#ifdef CNEARTREE_INSTRUMENTED
                     , iHeight, imultLeft, imultRight
#endif
                     , * NearTreeNodes, *ObjectStore, *ObjectCollide);
                } else {
                    NearTreeNodes->push_back(new CNearTree<unitcell>::NearTreeNode<unitcell>
                                             (ptLeft, ptRight, dMaxLeft, dMaxRight,pLeftBranch, pRightBranch,iTreeSize
#ifdef CNEARTREE_INSTRUMENTED
                                              , iHeight, imultLeft, imultRight
#endif
                                              , * NearTreeNodes, * ObjectStore, * ObjectCollide));
                }
                
            }
            if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted NearTreeNodes" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("Flags:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'Flags:' token" << std::endl;
                break;
            }
            
            serialin >> Flags; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted Flags" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DiamEstimate:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'DiamEstimate:' token" << std::endl;
                break;
            }
            
            serialin >> DiamEstimate; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted DiamEstimate" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("SumSpacings:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'SumSpacings:' token" << std::endl;
                break;
            }
            
            serialin >> SumSpacings; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted SumSpacings" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("SumSpacingsSq:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'SumSpacingsSq:' token" << std::endl;
                break;
            }
            
            serialin >> SumSpacingsSq; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted SumSpacings" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DimEstimate:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'DimEstimate:' token" << std::endl;
                break;
            }
            
            serialin >> DimEstimate; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted DimEstimate" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DimEstimateEsd:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'DimEstimateEsd:' token" << std::endl;
                break;
            }
            
            serialin >> DimEstimateEsd; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted DimEstimateEsd" << std::endl;
                break;
            }
            
#ifdef CNEARTREE_INSTRUMENTED
            serialin >> token; if (!serialin.good() || token != string("NodeVisits:")) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted, no 'NodeVisits:' token" << std::endl;
                break;
            }
            
            serialin >> NodeVisits; if (!serialin.good()) {
                std::cout << filenames[choiceAlgorithm] << " badly formatted NodeVisits" << std::endl;
                break;
            }
#endif
            cellTree[choiceAlgorithm-1] =
            new CNearTree <unitcell> (*DelayedIndices, *ObjectStore, *ObjectCollide, DeepestDepth,
                                      *NearTreeNodes, *BaseNode, Flags,DiamEstimate, SumSpacings, SumSpacingsSq,
                                      DimEstimate, DimEstimateEsd  // estimated dimension estimated standard deviation
#ifdef CNEARTREE_INSTRUMENTED
                                      , NodeVisits
#endif
                                      );
            gotckp = true;
            delete DelayedIndices;
            delete ObjectStore;
            delete ObjectCollide;
            delete NearTreeNodes;
            delete BaseNode;
            
        }
        break;
    }
    if (serialin.is_open()) serialin.close();
    if (gotckp) return;
    
    
    cellTree[choiceAlgorithm-1] = new CNearTree <unitcell> ();
    
	for (int i = 0; i < (int)num_rows; i++)
	{
        Cell rawcell(cellDArray[i][0], cellDArray[i][1], cellDArray[i][2], cellDArray[i][3], cellDArray[i][4], cellDArray[i][5]);
        mc = rawcell.LatSymMat66((spaceArray[i][0]).substr(0,1));
        primcell = mc*(rawcell.Cell2V6());
        if (!Reducer::Reduce(primcell,m,redprimcell,0.0)){
            std::cout << "Reduction failed for "<<idArray[i][0]<<" "<<
            cellDArray[i][3]<<" "<<
            cellDArray[i][4]<<" "<<
            cellDArray[i][5]<<" "<<
            cellDArray[i][0]<<" "<<
            cellDArray[i][1]<<" "<<
            cellDArray[i][2]<<" "<<
            spaceArray[i][0] << std::endl;
            std::cout << "Primitive G6 " << primcell[0]<<" "<<
            primcell[1]<<" "<<
            primcell[2]<<" "<<
            primcell[3]<<" "<<
            primcell[4]<<" "<<
            primcell[5] << std::endl;
        };
        searchcell = Cell(redprimcell).CellWithDegrees();
        unitcell cellData(searchcell[0],searchcell[1],searchcell[2],
                          searchcell[3],searchcell[4],searchcell[5],
                          0,0,0,0,0,0, (double)i);
        cellTree[choiceAlgorithm-1]->insert(cellData);
        
	}
    cellTree[choiceAlgorithm-1]->CompleteDelayedInsert();
    cellTree_itend[choiceAlgorithm-1] = cellTree[choiceAlgorithm-1]->end();
    cellTree[choiceAlgorithm-1]->Get_Checkpoint(&DelayedIndices,
                                                &ObjectStore,
                                                &ObjectCollide,
                                                &DeepestDepth,
                                                &NearTreeNodes,
                                                &BaseNode,
                                                &Flags,
                                                &DiamEstimate,
                                                &SumSpacings,
                                                &SumSpacingsSq,
                                                &DimEstimate,
                                                &DimEstimateEsd
#ifdef CNEARTREE_INSTRUMENTED
                                                , &NodeVisits
#endif
                                                );
    serialout.open(filenames[choiceAlgorithm].c_str(),std::ios::out|std::ios::trunc);
    if (serialout.is_open()) {
        serialout << "Metric: "<< choiceAlgorithm << std::endl;
        serialout << "DelayedIndices: { ";
        for (si = 0; si < DelayedIndices->size(); si++) {
            serialout << (*DelayedIndices)[si];
            if (si+1 < DelayedIndices->size()) serialout <<" ";
            if (si%64==63) serialout << std::endl;
        }
        serialout << " }" << std::endl;
        
        serialout << "ObjectStore: { ";
        for (si = 0; si < ObjectCollide->size(); si++) {
            ucell = (*ObjectStore)[si];
            ucell.getCell(cell,&row);
            serialout << "{ " << cell[0] << " "
            << cell[1] << " "
            << cell[2] << " "
            << cell[3] << " "
            << cell[4] << " "
            << cell[5] << " "
            << " " << row << " }"  << std::endl;
        }
        serialout << " }" << std::endl;
        
        serialout << "ObjectCollide: { ";
        for (si = 0; si < ObjectCollide->size(); si++) {
            sv = (*ObjectCollide)[si];
            putoutst(serialout,sv);
            if (si+1 < ObjectCollide->size()) serialout <<" ";
            if (si%64==63) serialout << std::endl;
        }
        serialout << " }" << std::endl;
        
        serialout << "DeepestDepth: " << DeepestDepth << std::endl;
        
        serialout << "NearTreeNodes: { ";
        
        serialout << "{ ";
        putoutst(serialout,BaseNode->m_ptLeft); serialout << " ";
        putoutst(serialout,BaseNode->m_ptRight); serialout << " ";
        serialout << BaseNode->m_dMaxLeft << " "<< BaseNode->m_dMaxRight << " ";
        putoutst(serialout,BaseNode->m_pLeftBranch); serialout << " ";
        putoutst(serialout,BaseNode->m_pRightBranch); serialout << " ";
        putoutst(serialout,BaseNode->m_iTreeSize); serialout << " ";
#ifdef CNEARTREE_INSTRUMENTED
        putoutst(serialout,BaseNode->m_iHeight); serialout << " ";
        putoutst(serialout,BaseNode->m_imultLeft); serialout << " ";
        putoutst(serialout,BaseNode->m_imultRight); serialout << " ";
#endif
        serialout << " }" << std::endl;
        
        for (si = 0; si < NearTreeNodes->size(); si++) {
            curntn= (*NearTreeNodes)[si];
            serialout << "{ ";
            putoutst(serialout,curntn->m_ptLeft); serialout << " ";
            putoutst(serialout,curntn->m_ptRight); serialout << " ";
            serialout << curntn->m_dMaxLeft << " " << curntn->m_dMaxRight << " ";
            putoutst(serialout,curntn->m_pLeftBranch); serialout << " ";
            putoutst(serialout,curntn->m_pRightBranch); serialout << " ";
            putoutst(serialout,curntn->m_iTreeSize); serialout << " ";
#ifdef CNEARTREE_INSTRUMENTED
            putoutst(serialout,curntn->m_iHeight); serialout << " ";
            putoutst(serialout,curntn->m_imultLeft); serialout << " ";
            putoutst(serialout,curntn->m_imultRight); serialout << " ";
#endif
            serialout << " }";
            if (si+1 < NearTreeNodes->size()) serialout <<std::endl;
        }
        serialout << " }" << std::endl;
        serialout << "Flags: " << Flags  << std::endl;
        serialout << "DiamEstimate: " << DiamEstimate  << std::endl;
        serialout << "SumSpacings: " << SumSpacings  << std::endl;
        serialout << "SumSpacingsSq: " << SumSpacingsSq  << std::endl;
        serialout << "DimEstimate: " << DimEstimate  << std::endl;
        serialout << "DimEstimateEsd: " << DimEstimateEsd  << std::endl;
#ifdef CNEARTREE_INSTRUMENTED
        serialout << "NodeVisits: " << NodeVisits  << std::endl;
#endif
        serialout.close();
        delete DelayedIndices;
        delete ObjectStore;
        delete ObjectCollide;
        delete NearTreeNodes;
        delete BaseNode;
        
    } else {
        std::cout << "unable to save database checkpoint file "<<filenames[choiceAlgorithm] << std::endl;
    }
}


//*****************************************************************************
void NearestInputReport( std::ostream& out, const arma::vec6& probeArray, const arma::vec6& primredprobe )
{
    out << "Raw Unknown Cell [" <<
    probeArray[0] << ", " <<
    probeArray[1] << ", " <<
    probeArray[2] << ", " <<
    probeArray[3] << ", " <<
    probeArray[4] << ", " <<
    probeArray[5] <<
    "] Lattice: "<< probelattice <<  std::endl;
    out << "Reduced Primitive Cell: [" <<
    primredprobe[0] << ", " <<
    primredprobe[1] << ", " <<
    primredprobe[2] << ", " <<
    primredprobe[3] << ", " <<
    primredprobe[4] << ", " <<
    primredprobe[5] << "]" << std::endl;
}

//*****************************************************************************
void findNearest( int k, double distance )
{
    vector <unitcell> myvector;
    vector <size_t> myindices;
    vector <double> mydistances;
    long spheredata;
    double dtest;

    std::cout << "entering findNearest " << k << distance << std::endl;  
    unitcell unknownCell = unitcell(primredprobe[0], primredprobe[1], primredprobe[2], primredprobe[3], primredprobe[4], primredprobe[5], 0, 0, 0, 0, 0, 0, 0);
    NearestInputReport( std::cout, probeArray, primredprobe );

    std::cout <<  "crootvol: " << crootvol << std::endl;
    dtest = distance;
    // if (distance > crootvol*.025 || distance <= 0.) dtest = crootvol*.025;
    if (distance <= 0.) dtest = 1.e38;
    if (distance > 0. && dtest > distance) dtest = distance;

    std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;
    spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
                                                                     myindices,mydistances,unknownCell);
    std::cout <<"returned spheredata " << spheredata << std::endl;
    /*if (spheredata <  k && (dtest < distance || distance <= 0.) )  {
	dtest = distance;
	if (dtest <= 0.) dtest = crootvol;
	std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;
        spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
			myindices,mydistances,unknownCell);
	std::cout <<"returned spheredata " << spheredata << std::endl;

        if (spheredata <  (k+1)/2 && (dtest < distance || distance <= 0.) )  {
		dtest = crootvol*2.;
		std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;
		spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
					                        myindices,mydistances,unknownCell);
		std::cout <<"returned spheredata " << spheredata << std::endl;

	    if (spheredata <  1 )  {
		    dtest = crootvol*5.;
		    std::cout <<"calling FindK_NearestNeighbors " << "k: "<<k << " dtest: " << dtest << std::endl;
		    spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,dtest, myvector,
	                                                        myindices,mydistances,unknownCell);
		    std::cout <<"returned spheredata " << spheredata << std::endl;
	        if (spheredata < 1) {
                    spheredata = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(k,1.e38, myvector,
                                                                myindices,mydistances,unknownCell);
		}
	    }
        }
    }*/
    if (spheredata != 0) {
        std::cout << "Depth: " << cellTree[choiceAlgorithm-1]->GetDepth() << std::endl;
        SphereResults( std::cout, myvector, myindices, mydistances, unknownCell);
        
        if (!sauc_batch_mode)
            std::cout << "File name if you want the output saved" << std::endl;
        
        std::string s;
        std::getline( std::cin, s );
        std::cin.clear();
        if ( ! s.empty() )
        {
            std::string filename;
            std::stringstream ss(s);
            if ( ss >> filename )
            {
                std::cout << " ";
                std::ofstream output( filename.c_str() );
                NearestInputReport( output, probeArray, primredprobe );
                SphereResults( output, myvector, myindices, mydistances, unknownCell);
		output.close();
                if (sauc_batch_mode)
                    std::cout << "Output saved to: " << filename << std::endl;
            }
        }
    }
    
}

//*****************************************************************************
void SphereResults( std::ostream& out,
                   const std::vector<unitcell>&  myvector,
                   const std::vector<size_t>&    myindices,
                   const std::vector<double>&    mydistances,
                   const unitcell&               unknownCell)
{
    long entry;
    string head;
    vector<long> myentries;
    vector<long> myfamilies;
    vector<long> myfamiliesordinal;
    vector<long> myfamilylist;
    vector<long> myfamilycount;
    vector<long> mythread;
    myentries.assign(myvector.size(),-1L);   /* The entry with this PDB ID */
    myfamilies.assign(myvector.size(),-1L);  /* The base entry with the same header */
    myfamiliesordinal.assign(myvector.size(),-1L);/* ordinal in the family list */
    myfamilylist.assign(myvector.size(),-1L);/* List in distance order of first
                                         entry in each family */
    myfamilycount.assign(myvector.size(),-1L);/* counts of hits in each family */
    mythread.assign(myvector.size(),-1L);     /* links on hashed headers */
    long ind, family_size;
    long nextthread, prevthread, firstthread, numhit, thread[257];
    int ii;
    family_size=0;
    for (ii=0; ii < 257; ii++) thread[ii] = -1;
    out << "\nSphere Results " << myvector.size() << " Cells" <<std::endl;
    /* find the header threads of the results,
       this depends on the assumption that the original
       list is sorted by distance
     */
    for (ind=0; ind < (long)myvector.size(); ind++) {
        myentries[ind] = myfamilies[ind] = myfamiliesordinal[ind] = myfamilylist[ind] = myfamilycount[ind] = mythread[ind] = -1;
    }
    for (ind=0; ind < (long)myvector.size(); ind++) {
        const unitcell * const cell = & myvector[ind];
        numRow = (int)(*cell).getRow();
        entry = findEntryDatabase(idArray[numRow][0]);
        if (entry < 0) {
            std::cout << "entry for "<< idArray[numRow][0] << " is negative" << std::endl;
            myentries[ind] = -1;
            myfamilies[ind] = -1;
        } else {
            myentries[ind] = entry;
            myfamilies[ind] = findHeadDatabase(entryArray[entry][1]);
            if (myfamilies[ind] < 0 ) {
                std::cout << "head for "<< entryArray[entry][1] << " is negative" << std::endl;
            }
        }
        ii = (myfamilies[ind])&0xFF;  /* hash code for families */
        if (entry < 0) ii = 256;
        nextthread = thread[ii];
        if (nextthread < 0) {
            thread[ii] = ind;
            myfamiliesordinal[ind] = family_size;
            mythread[ind] = -1;
            myfamilylist[family_size] = ind;
            myfamilycount[family_size] = 1L;
            family_size++;
        } else {
            firstthread = -1;
            prevthread = -1;
            numhit = 0;
            while (nextthread >= 0) {
                prevthread = nextthread;
                nextthread = mythread[prevthread];
                if (myfamilies[prevthread] == myfamilies[ind]) {
                    if (numhit==0) firstthread = prevthread;
                    numhit++;
                    myfamiliesordinal[ind] = myfamiliesordinal[prevthread];
                }
            }
            if (numhit==0) {
                myfamiliesordinal[ind] = family_size;
                myfamilylist[family_size] = ind;
                myfamilycount[family_size] = 1L;
                family_size++;
            } else {
                myfamiliesordinal[ind] = myfamiliesordinal[firstthread];
                myfamilycount[myfamiliesordinal[ind]]++;
            }
            mythread[prevthread] = ind;
            mythread[ind] = -1;
        }
    }

    if (noCSD) {
        out << "Found " << family_size << " families organized by PDB header" << std::endl << std::endl;
    } else {
        out << "Found " << family_size << " families organized by PDB header or CSD Refcode" << std::endl << std::endl;
    }    

    for (ii = 0; ii < family_size; ii++) {
        long myfamily;
        long familyordinal;
        ind = myfamilylist[ii];
        myfamily = myfamilies[ind];
        familyordinal = 0;
        string pdbid;
        while (ind >= 0) {
            const unitcell * const cell = & myvector[ind];
            numRow = (int)(*cell).getRow();
            if (sauc_javascript) {
                if (idArray[numRow][0].length() > 4) {
                    pdbid = "<b><a href=\"https://summary.ccdc.cam.ac.uk/structure-summary?refcode=" +
                    idArray[numRow][0] + "\" target=\"_blank\">" + idArray[numRow][0] + "</a></b>";
                    
                } else {
                    pdbid = "<b><a href=\"http://www.rcsb.org/pdb/explore.do?structureId=" +
                    idArray[numRow][0] + "\" target=\"_blank\">" + idArray[numRow][0] + "</a></b>";
                }
            } else {
               pdbid = idArray[numRow][0];
            }
            familyordinal++;
            if (familyordinal == 1) {
                entry = findEntryDatabase(idArray[numRow][0]);
                if (entry >= 0) {
                    out << ii+1 << ": "<< entryArray[entry][1];
                } else {
                    out << ii+1 << ": Unidentified Header";
                }
                if (myfamilycount[ii] > 1) {
                    out << " [" << myfamilycount[ii] << " cells found] ";
                    if (sauc_javascript) {
                      out << "<a href=\"javascript:open_close('famfloat"<< ii+1 << "');\">(expand/collapse)</a>";
                    }
                }
                out << std::endl;
            }
            if (familyordinal == 2 && sauc_javascript) {
                out << "<div id=\"famfloat" << ii+1 <<
                "\" style=\"display:none\">";
            }
            out << "    " << familyordinal << ": " <<
            pdbid << " Dist: " <<
            mydistances[ind] << " Cell: [" <<
            cellDArray[numRow][0] << ", " <<
            cellDArray[numRow][1] << ", " <<
            cellDArray[numRow][2] << ", " <<
            cellDArray[numRow][3] << ", " <<
            cellDArray[numRow][4] << ", " <<
            cellDArray[numRow][5] << "], SG: " <<
            spaceArray[numRow][0];
            if (zArray[numRow][0] > 0) {
            out << ", Z: " <<
            zArray[numRow][0] << " ";
            } else {
                out << ", ";
            }
            out << " Prim. Red. Cell: ["<<
            (*cell).getData(0) << ", " <<
            (*cell).getData(1) << ", " <<
            (*cell).getData(2) << ", " <<
            (*cell).getData(3) << ", " <<
            (*cell).getData(4) << ", " <<
            (*cell).getData(5) << "]" << std::endl;
            entry = findEntryDatabase(idArray[numRow][0]);
            if (entry >= 0) {
                if (entryArray[entry][3].length() > 0)
                    out << "      " << "Compound: " << entryArray[entry][3] << std::endl;
                if (entryArray[entry][4].length() > 0)
                    out << "      " << "Source: " << entryArray[entry][4];
                if (entryArray[entry][6].length() > 0)
                    out << ", Res: " << entryArray[entry][6];
                if (entryArray[entry][7].length() > 0)
                    out << " " << entryArray[entry][7];
                out << std::endl;
            }
            ind = mythread[ind];
            while (ind >= 0) {
                if (myfamilies[ind] == myfamily) break;
                ind = mythread[ind];
            }
        }
        if (myfamilycount[ii] > 1 && sauc_javascript) {
            out << "</div>";
        }
        out << std::endl;
    }
        
}

//*****************************************************************************
void SphereInputReport( std::ostream& out )
{
    out << "Raw Unknown Cell: ["<<
    probeArray[0] << ", " <<
    probeArray[1] << ", " <<
    probeArray[2] << ", " <<
    probeArray[3] << ", " <<
    probeArray[4] << ", " <<
    probeArray[5] <<
    "] Lattice: "<< probelattice <<  std::endl;
    out << "Primitive Reduced Cell: [" <<
    primredprobe[0] << ", " <<
    primredprobe[1] << ", " <<
    primredprobe[2] << ", " <<
    primredprobe[3] << ", " <<
    primredprobe[4] << ", " <<
    primredprobe[5] << "]" << std::endl;
}

//*****************************************************************************
void findSphere( int limit )
{
    unitcell unknownCell = unitcell(primredprobe[0], primredprobe[1], primredprobe[2], primredprobe[3], primredprobe[4], primredprobe[5], 0, 0, 0, 0, 0, 0, 0);
    
	std::cout << std::endl;
    SphereInputReport( std::cout );
	vector <unitcell> myvector;
    vector <size_t> myindices;
    vector <double> mydistances;
	const long sphereData = cellTree[choiceAlgorithm-1]->FindK_NearestNeighbors(limit,sphereRange, myvector,
                                                                                myindices,mydistances,unknownCell);
    
    SphereResults( std::cout, myvector, myindices, mydistances, unknownCell);
    
    if (!sauc_batch_mode)
        std::cout << "File name if you want the output saved" << std::endl;
    
    std::string s;
    std::getline( std::cin, s );
    std::cin.clear();
    if ( ! s.empty() )
    {
        std::string filename;
        std::stringstream ss(s);
        if ( ss >> filename )
        {
            std::cout << " ";
            std::ofstream output( filename.c_str() );
            SphereInputReport( output );
            SphereResults( output, myvector, myindices, mydistances, unknownCell);
            output.close();
            if (sauc_batch_mode)
                std::cout << "Output saved to: " << filename << std::endl;
        }
    }
}

//*****************************************************************************
void findRange( void )
{
	std::cout << std::endl;
	std::cout << "Unknown Cell\n" <<
    "A: "     << probeArray[0] << " " <<
    "B: "     << probeArray[1] << " " <<
    "C: "     << probeArray[2] << " " <<
    "Alpha: " << probeArray[3] << " " <<
    "Beta: "  << probeArray[4] << " " <<
    "Gamma: " << probeArray[5] << std::endl;
	std::cout << "\nRange Results\n";
	for (int i = 0; i < (int)num_rows; i++)
	{
		if ((probeArray[0] + numRangeA    ) >= cellDArray[i][0] && (probeArray[0] - numRangeA    ) <= cellDArray[i][0] &&
			(probeArray[1] + numRangeB    ) >= cellDArray[i][1] && (probeArray[1] - numRangeB    ) <= cellDArray[i][1] &&
			(probeArray[2] + numRangeC    ) >= cellDArray[i][2] && (probeArray[2] - numRangeC    ) <= cellDArray[i][2] &&
			(probeArray[3] + numRangeAlpha) >= cellDArray[i][3] && (probeArray[3] - numRangeAlpha) <= cellDArray[i][3] &&
			(probeArray[4] + numRangeBeta ) >= cellDArray[i][4] && (probeArray[4] - numRangeBeta ) <= cellDArray[i][4] &&
			(probeArray[5] + numRangeGamma) >= cellDArray[i][5] && (probeArray[5] - numRangeGamma) <= cellDArray[i][5])
		{
			std::cout << "PDBID: " << idArray[i][0] << " " <<
            "A: "     << cellDArray[i][0] << " " <<
            "B: "     << cellDArray[i][1] << " " <<
            "C: "     << cellDArray[i][2] << " " <<
            "Alpha: " << cellDArray[i][3] << " " <<
            "Beta: "  << cellDArray[i][4] << " " <<
            "Gamma: " << cellDArray[i][5] << " " <<
            "Space Group: " << spaceArray[i][0];
            
            if (zArray[i][0] > 0) {
                std::cout << ", Z: " <<
                zArray[i][0] << std::endl;
            } else {
                std::cout << std::endl;
            }
		}
	}
}

//*****************************************************************************
int main ()
{
    
    // Check for sauc html run
    
    if (std::getenv("SAUC_BATCH_MODE")) {
        sauc_batch_mode = 1;
    }

    if (std::getenv("SAUC_JAVASCRIPT")) {
        sauc_javascript = 1;
    }

    //Create Database
    
    filenames[0] = "PDBcelldatabase.csv";
    filenames[1] = "PDBcellneartreeL1.dmp";
    filenames[2] = "PDBcellneartreeL2.dmp";
    filenames[3] = "PDBcellneartreeNCDist.dmp";
    filenames[4] = "PDBcellneartreeV7.dmp";
    filenames[5] = "PDBcelldatabase.dmp";
    CSDfilenames[0] = "CSDcelldatabase.csv";
    CSDfilenames[1] = "CSDcelldatabase.dmp";

    makeDatabase(filenames[0]);
    makeEntryDatabase("entries.idx");
    noCSD = makeCSDDatabase(CSDfilenames[0],CSDfilenames[1]);
    
	unitcell cell;
    
    std::cout << "sauc Copyright (C) Keith McGill 2013, 2014" << std::endl;
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY" << std::endl;
    std::cout << "This is free software, and you are welcome to" << std::endl;
    std::cout << "redistribute it under the GPL or LGPL" << std::endl;
    std::cout << "See the program documentation for details" << std::endl;
    std::cout << "Rev 0.8, 24 Apr 2014, Mojgan Asadi, Herbert J. Bernstein" << std::endl;
    std::cout << "Rev 0.9, 18 Jul 2015, Herbert J. Bernstein" << std::endl;
    std::cout << "Rev 0.9.1, 16 Mar 2016, Herbert J. Bernstein" << std::endl;
    
	while (endProgram != 1)
	{
        if (goBack != 1)
        {
            int ii;
            double edgemax;
            for (ii=0; ii < 6; ii++) probeArray[ii] = 0;
        	std::cout << "Search of Alternate Unit Cells\n";
            if (!sauc_batch_mode) std::cout << "\nPlease Input Your Data\n";
            if (!sauc_batch_mode) std::cout << "Lattice Centering (P, A, B, C, F, I, R, H, V): ";
            std::cin >> probelattice; std::cin.clear();
            if (!sauc_batch_mode) std::cout << "A: ";
        	std::cin >> probeArray[0]; std::cin.clear();
            if (!sauc_batch_mode) std::cout << "B: ";
        	std::cin >> probeArray[1]; std::cin.clear();
            if (!sauc_batch_mode) std::cout << "C: ";
        	std::cin >> probeArray[2]; std::cin.clear();
            if (!sauc_batch_mode) std::cout << "Alpha: ";
        	std::cin >> probeArray[3]; std::cin.clear();
            if (!sauc_batch_mode) std::cout << "Beta: ";
        	std::cin >> probeArray[4]; std::cin.clear();
            if (!sauc_batch_mode) std::cout << "Gamma: ";
        	std::cin >> probeArray[5]; std::cin.clear();
            std::cout << std::endl;
            if (sauc_batch_mode) {
                std::cout << "Centering and Probe cell :"<<  probelattice << " "
                << probeArray[0] << " "
                << probeArray[1] << " "
                << probeArray[2] << " "
                << probeArray[3] << " "
                << probeArray[4] << " "
                << probeArray[5] << std::endl;
            }
            edgemax = 0;
            for (ii=0; ii < 6; ii++) {
                if (ii<3 && probeArray[ii] > edgemax)edgemax = probeArray[ii];
                if (probelattice.size() > 0
                    && (probelattice[0]=='V' || probelattice[0]=='v')) {
                    if ((ii<3 && probeArray[ii] <= 1.)
                        || (ii > 2
                            && std::abs(probeArray[ii]) > edgemax*1.01)) {
                            std::cout << "Invalid G6 cell "
                            << probeArray[0] << " "
                            << probeArray[1] << " "
                            << probeArray[2] << " "
                            << probeArray[3] << " "
                            << probeArray[4] << " "
                            << probeArray[5] << " "
                            << std::endl;
                            continue;
                        }
                } else if ((ii< 3 && probeArray[ii]<=1.)
                           ||( ii >2 && std::abs(probeArray[ii]-90.)> 75)) {
                    std::cout << "Invalid "<<probelattice<<" cell "
                    << probeArray << std::endl;
                    continue;
                }
            }
        }
        makeprimredprobe();
        
        while (quitAlgorithm != 1 && endProgram != 1)
        {
            goBack = 0;
            if (!sauc_batch_mode) std::cout << "1. L1";
            if (!sauc_batch_mode) std::cout << "\n2. L2";
            if (!sauc_batch_mode) std::cout << "\n3. NCDist";
            if (!sauc_batch_mode) std::cout << "\n4. V7";
            if (!sauc_batch_mode) std::cout << "\n5. Quit";
            if (!sauc_batch_mode) std::cout << "\nChoose a Metric: ";
            priorAlgorithm = choiceAlgorithm;
            std::cin >> choiceAlgorithm; std::cin.clear();
            std::cin.ignore(100000,'\n');
            
            if (sauc_batch_mode && choiceAlgorithm > 0 && choiceAlgorithm < 5 ) {
                switch (choiceAlgorithm) {
                    case 1: std::cout << "L1 metric search algorithm" << std::endl; break;
                    case 2: std::cout << "L2 metric search algorithm" << std::endl; break;
                    case 3: std::cout << "NCDist metric search algorithm" << std::endl; break;
                    case 4: std::cout << "V7 metric search algorithm" << std::endl; break;
                }
            }
            
            if (choiceAlgorithm == 1 && priorAlgorithm!= 1)
            {
                cell.changeOperator(1);
                cell.changeAlgorithm(1);
                cell.changeScaledist(1./std::sqrt(6.));
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 2 && priorAlgorithm!= 2)
            {
                cell.changeOperator(1);
                cell.changeAlgorithm(2);
                cell.changeScaledist(1.);
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 3 && priorAlgorithm!= 3)
            {
                cell.changeOperator(2);
                cell.changeAlgorithm(3);
                cell.changeScaledist(0.1);
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 4 && priorAlgorithm!= 4)
            {
                cell.changeOperator(2);
                cell.changeAlgorithm(4);
                cell.changeScaledist(std::sqrt(6./7.));
                quitAlgorithm = 1;
                //Build Tree
                if (!cellTree[choiceAlgorithm-1]) buildNearTree();
            }
            else if (choiceAlgorithm == 5 && priorAlgorithm!= 5)
            {
                quitAlgorithm = 1;
                endProgram = 1;
                quitContinue = 1;
            }
            else if (choiceAlgorithm < 1 || choiceAlgorithm > 5)
            {
                std::cout << "Incorrect Choice. Please Choose Again.\n";
                choiceAlgorithm = priorAlgorithm;
            }
            else
            {
                quitAlgorithm = 1;
            }
            std::cout << std::endl;
        }
        quitAlgorithm = 0;
        
        while (quitSimilar != 1 && endProgram != 1)
        {
            if (!sauc_batch_mode) std::cout << "1. Nearest";
            if (!sauc_batch_mode) std::cout << "\n2. Sphere";
            if (!sauc_batch_mode) std::cout << "\n3. Range";
            if (!sauc_batch_mode) std::cout << "\n4. Back";
            if (!sauc_batch_mode) std::cout << "\n5. Quit";
            if (!sauc_batch_mode) std::cout << "\nChoose A Similarity: ";
            std::cin >> choiceSimilar;
            std::cin.clear(); std::cin.ignore(10000,'\n');
            if (sauc_batch_mode && choiceSimilar > 0 && choiceSimilar < 5) {
                switch(choiceSimilar) {
                    case 1:  std::cout << "Finding Nearest Cell" << std::endl; break;
                    case 2:  std::cout << "Searching in a Sphere" << std::endl; break;
                    case 3:  std::cout << "Brute Force Range Search" << std::endl; break;
                }
            }
            if (choiceSimilar == 1)
            {
                findNearest(1,0.);
                quitSimilar = 1;
            }
            else if (choiceSimilar == 2)
            {
                string token;
                string sradline;
                int limit, klimit;
                int ll;
		klimit = 10;
                if (!sauc_batch_mode) std::cout << "\nPlease Input Your Sphere's Radius and Optional Limit K: ";
                std::getline(std::cin,sradline);
                std::stringstream ssradline(sradline);
                ssradline >> token;
                ll = token.length();
                if (ll > 1 && token[ll-1]=='%') {
                    sphereRange=convertToDouble(token.substr(0,ll-1))*crootvol/100.;
                    cout << "crootvol: " << crootvol << endl;
                    cout << "sphereRange: " << sphereRange << endl;
                } else {
                    sphereRange=convertToDouble(token);
                }
                if (ssradline >> limit) {
                  if (limit >= 1) klimit = limit;
                } else {
                  klimit = 100000;
                }
                if (sauc_batch_mode) {
                    std::cout << "Sphere Radius: "<< sphereRange << " Limit: " << klimit << std::endl;
                }
                findNearest(klimit,sphereRange);
                quitSimilar = 1;
            }
            else if (choiceSimilar == 3)
            {
                if (!sauc_batch_mode) std::cout << "\nPlease Input Your Ranges\n";
                if (!sauc_batch_mode) std::cout << "A: ";
                std::cin >> numRangeA; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "B: ";
                std::cin >> numRangeB; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "C: ";
                std::cin >> numRangeC; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "Alpha: ";
                std::cin >> numRangeAlpha; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "Beta: ";
                std::cin >> numRangeBeta; std::cin.clear();
                if (!sauc_batch_mode) std::cout << "Gamma: ";
                std::cin >> numRangeGamma; std::cin.clear();
                std::cin.clear();
                std::cin.ignore(100000,'\n');
                if (sauc_batch_mode) {
                    std::cout << "A, B, C, Alpha, Beta, Gamma ranges: "
                    << numRangeA << ", "
                    << numRangeB << ", "
                    << numRangeC << ", "
                    << numRangeAlpha << ", "
                    << numRangeBeta << ", "
                    << numRangeGamma << std::endl;
                }
                findRange();
                quitSimilar = 1;
            }
            else if (choiceSimilar == 4)
            {
                quitSimilar = 1;
                goBack = 1;
            }
            else if (choiceSimilar == 5)
            {
                quitSimilar = 1;
                endProgram = 1;
                quitContinue = 1;
            }
            else
            {
                if (sauc_batch_mode) {
                    quitSimilar = 1;
                    endProgram = 1;
                    quitContinue = 1;
                } else {
                    std::cout << "Inncorrect Choice. Please Choose Again.\n";
                }
            }
            std::cout << std::endl;
        }
        quitSimilar = 0;
        
        while (quitContinue == 0)
        {
            if (!sauc_batch_mode) std::cout << "1. Input New Values";
            if (!sauc_batch_mode) std::cout << "\n2. Choose a New Metric";
            if (!sauc_batch_mode) std::cout << "\n3. Choose a New Similarity";
            if (!sauc_batch_mode) std::cout << "\n4. Quit";
            if (!sauc_batch_mode) std::cout << "\nChoice: ";
            std::cin >> choiceContinue; std::cin.clear();
            std::cin.ignore(100000,'\n');
            if (choiceContinue == 1)
            {
                goBack = 0; //Added so that it would loop back to main menu
                quitContinue = 1;
            }
            else if (choiceContinue == 2)
            {
                goBack = 1;
                quitContinue = 1;
            }
            else if (choiceContinue == 3)
            {
                goBack = 1;
                quitAlgorithm = 1;
                quitContinue = 1;
            }
            else if (choiceContinue == 4)
            {
                endProgram = 1;
                quitContinue = 1;
            }
            else
            {
                if (sauc_batch_mode) {
                    endProgram = 1;
                    quitContinue = 1;
                } else {
                    std::cout << "Inncorrect Choice. Please Choose Again.\n";
                }
            }
            std::cout << std::endl;
        }
        quitContinue = 0;
    }
    
	return 0;
}
