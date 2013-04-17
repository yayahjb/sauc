/* sauc.cpp  -- Search of Alternate Unit Cells
   (C) Copyright Keith McGill 2013
 
   rev 27 Mar 2013 -- HJB

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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <iostream>
#include "TNear.h"
#include "triple.h"
#include "rhrand.h"
#include "unitcell.h"
#include <fstream>
#include <string>
#include "V7.h"
#include "Cell.h"
#include "Reducer.h"
#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK
#include <armadillo>
#include <vector>


using namespace std;

CNearTree <unitcell> * cellTree[4] = {NULL,NULL,NULL,NULL};
CNearTree <unitcell>::iterator cellTree_itend[4];
string filenames[5];

const int NUM_COLUMNS = 9, NUM_ROWS = 75325, NUM_DUMP = 2;
//const int NUM_COLUMNS = 9, NUM_ROWS = 10000, NUM_DUMP = 2;
//const int NUM_ROWS = 100;

string idArray[NUM_ROWS][1];
double cellDArray[NUM_ROWS][6];
string spaceArray[NUM_ROWS][1];
int zArray[NUM_ROWS][1];
double probeArray[6];
arma::vec6 primredprobe;
string probelattice;

int nearest = 0, numRow = 0;
int quitAlgorithm = 0, quitSimilar = 0, quitContinue = 0, endProgram = 0, choiceAlgorithm = 0, choiceSimilar = 0, choiceContinue = 0, goBack = 0,
    priorAlgorithm = -1;
double result0 = 0, result1 = 0, result2 = 0, result3 = 0, result4 = 0, result5 = 0, resultSumTemp = 0, resultSum = 0, numRange = 0;
double numRangeA, numRangeB, numRangeC, numRangeAlpha, numRangeBeta, numRangeGamma, sphereRange;
string valueDump;

double convertToDouble(string value)
{
	int numDot = 0, numDotCount = 0;
	double number = 0, numTotal = 0, numDec = 0, countDec = 10;
	char charValue;

	for (int i = 0; i < (int)value.length(); i++)
	{
		charValue = value.at(i);

		switch(charValue)
		{
		case '0':
			number = 0;
			break;
		case '1':
			number = 1;
			break;
		case '2':
			number = 2;
			break;
		case '3':
			number = 3;
			break;
		case '4':
			number = 4;
			break;
		case '5':
			number = 5;
			break;
		case '6':
			number = 6;
			break;
		case '7':
			number = 7;
			break;
		case '8':
			number = 8;
			break;
		case '9':
			number = 9;
			break;
		case '.':
			numDot = 1;
			break;
		}
			
		if (numDot != 1 && i == 0)
		{
			numTotal = number;
		}
		else if (numDot != 1)
		{
			numTotal = (numTotal * 10) + number;
		}
		else if (numDot == 1 && numDotCount == 1)
		{
			numDec = number / countDec;
			numTotal = numTotal + numDec;
			countDec *= 10;
		}
		if (numDot == 1 && numDotCount == 0)
		{
			numDotCount++;
		}
	}
	return(numTotal);
}

int convertToInt(string zvalue)
{
	int number = 0, numTotal = 0;
	char charValue;

	for (int i = 0; i < (int)zvalue.length(); i++)
	{
		charValue = zvalue.at(i);

		switch(charValue)
		{
		case '0':
			number = 0;
			break;
		case '1':
			number = 1;
			break;
		case '2':
			number = 2;
			break;
		case '3':
			number = 3;
			break;
		case '4':
			number = 4;
			break;
		case '5':
			number = 5;
			break;
		case '6':
			number = 6;
			break;
		case '7':
			number = 7;
			break;
		case '8':
			number = 8;
			break;
		case '9':
			number = 9;
			break;
		}
			
		if (i == 0)
		{
			numTotal = number;
		}
		else
		{
			numTotal = (numTotal * 10) + number;
		}
	}
	return(numTotal);
}

void makeDatabase(string filename)
{
	string format;
	
	ifstream infile;
	infile.open(filename.c_str());
	for (int i = 0; i < NUM_DUMP; i++)
	{
		getline (infile, valueDump, 'r');
		//cout << valueDump << std::endl;
	}
	for (int i = 0; i < NUM_ROWS; i++)
	{
		string value;

		getline (infile, value, ',');
		//cout << value << std::endl;
		idArray[i][0] = string(value, 2, value.length()-3);
		//cout << idArray[i][0] << std::endl;

		getline (infile, value, ',');
		//cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//cout << value << std::endl;
		cellDArray[i][3] = convertToDouble(value);
		//cout << cellDArray[i][3] << std::endl;

		getline (infile, value, ',');
		//cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//cout << value << std::endl;
		cellDArray[i][4] = convertToDouble(value);
		//cout << cellDArray[i][4] << std::endl;

		getline (infile, value, ',');
		//cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//cout << value << std::endl;
		cellDArray[i][5] = convertToDouble(value);
		//cout << cellDArray[i][5] << std::endl;

		getline (infile, value, ',');
		//cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//cout << value << std::endl;
		cellDArray[i][0] = convertToDouble(value);
		//cout << cellDArray[i][0] << std::endl;

		getline (infile, value, ',');
		//cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//cout << value << std::endl;
		cellDArray[i][1] = convertToDouble(value);
		//cout << cellDArray[i][1] << std::endl;

		getline (infile, value, ',');
		//cout << value << std::endl;
		value = string(value, 1, value.length()-2);
		//cout << value << std::endl;
		cellDArray[i][2] = convertToDouble(value);
		//cout << cellDArray[i][2] << std::endl;

		getline (infile, value, ',');
		//cout << value << std::endl;
		spaceArray[i][0] = string(value, 1, value.length()-2);
		//cout << spaceArray[i][0] << std::endl;

		getline (infile, valueDump, '"');
		getline (infile, value, '"');
		//cout << value << std::endl;
		value = string(value, 0, value.length());
		//cout << value << std::endl;
		zArray[i][0] = convertToInt(value);
		//cout << zArray[i][0] << std::endl;

		//cout << "-----------------------------------------------" << std::endl;
	}
	infile.close();
}

bool makeprimredprobe(void)
{
    string latsym;
    char clatsym;
    arma::vec6 v6cell;
    arma::vec6 redprimcell;
    arma::mat66 mc;
    arma::mat66 m;
    arma::vec6 primcell;
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
        default:
            cerr << "Unrecognized lattice symbol "<< probelattice<<" treated as P" << std::endl;
            latsym = "P";
            mc = rawcell.LatSymMat66(latsym);
            primcell = mc*(rawcell.Cell2V6());
            break;
    }
    ret = Reducer::Reduce(primcell,m,redprimcell,0.0);
    primredprobe = Cell(redprimcell).CellWithDegrees();
    cout << "Primitive Reduced Probe Cell: " <<
      primredprobe[0]<<" "<<
      primredprobe[1]<<" "<<
      primredprobe[2]<<" "<<
      primredprobe[3]<<" "<<
      primredprobe[4]<<" "<<
      primredprobe[5] << std::endl;
    return ret;
}

#define putoutst(serialout,value) \
    if (value != ULONG_MAX) { \
        (serialout) << (value); \
    } else { \
        (serialout) << "-1"; \
    } \


void buildNearTree(void)
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
	int count = 0;
    bool gotckp = false;
    
    serialin.open(filenames[choiceAlgorithm].c_str(),std::ios::in);
    while (true) {
        string token;
        int Algorithm = 0;
        int itoken;
        if (serialin.is_open()) {
            cout << "Reading database " << filenames[choiceAlgorithm] << endl;
            serialin >> token; if (!serialin.good() || token != string("Algorithm:")) {
                cout << filenames[choiceAlgorithm] << "badly formatted, no 'Algorithm:' token" << std::endl;
                break;
            }
            serialin >> Algorithm;
            if (!serialin.good() || algorithm != choiceAlgorithm) {
                cout << filenames[choiceAlgorithm] << "badly formatted, algorithm value wrong" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DelayedIndices:")) {
                cout << filenames[choiceAlgorithm] << "badly formatted, no 'DelayedIndices:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                cout << filenames[choiceAlgorithm] << "badly formatted, no DelayedIndices '{' token" << std::endl;
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
                cout << filenames[choiceAlgorithm] << "badly formatted DelayedIndices" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("ObjectStore:")) {
                cout << filenames[choiceAlgorithm] << "badly formatted, no 'ObjectStore:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                cout << filenames[choiceAlgorithm] << "badly formatted, no ObjectStore '{' token" << std::endl;
                break;
            }
            ObjectStore = new std::vector<unitcell>();
            while (serialin.good()) {
                double cell[6];
                double row;
                serialin >> token;
                if (token == string("}")) break;
                if (!serialin.good() || token != string("{")) {
                    cout << filenames[choiceAlgorithm] << "badly formatted, missing ObjectStore '{' token" << std::endl;
                    break;
                }
                serialin >> cell[0] >> cell[1] >> cell[2] >> cell[3] >> cell[4] >> cell[5] >> row;
                if (!serialin.good()) {
                    cout << filenames[choiceAlgorithm] << "badly formatted, bad object" << std::endl;
                    break;
                }
                ObjectStore->push_back(unitcell(cell,row));
                serialin >> token; if (!serialin.good() || token != string("}")) {
                    cout << filenames[choiceAlgorithm] << "badly formatted, missing ObjectStore '}' token" << std::endl;
                    break;
                }
                
            }
            if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << "badly formatted ObjectStore" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("ObjectCollide:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'ObjectCollide:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no ObjectCollide '{' token" << std::endl;
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
                cout << filenames[choiceAlgorithm] << " badly formatted ObjectCollide" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DeepestDepth:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'DeepestDepth:' token" << std::endl;
                break;
            }
            
            serialin >> DeepestDepth; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted DeepestDepth" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("NearTreeNodes:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'NearTreeNodes:' token" << std::endl;
                break;
            }
            serialin >> token; if (!serialin.good() || token != string("{")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no NearTreeNodes '{' token" << std::endl;
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
                    cout << filenames[choiceAlgorithm] << " badly formatted, no NearTreeNode '{' token " << token << std::endl;
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
                    cout << filenames[choiceAlgorithm] << " badly formatted, no NearTreeNode '}' token: " << token <<std::endl;
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
                cout << filenames[choiceAlgorithm] << " badly formatted NearTreeNodes" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("Flags:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'Flags:' token" << std::endl;
                break;
            }
            
            serialin >> Flags; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted Flags" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DiamEstimate:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'DiamEstimate:' token" << std::endl;
                break;
            }
            
            serialin >> DiamEstimate; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted DiamEstimate" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("SumSpacings:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'SumSpacings:' token" << std::endl;
                break;
            }
            
            serialin >> SumSpacings; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted SumSpacings" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("SumSpacingsSq:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'SumSpacingsSq:' token" << std::endl;
                break;
            }
            
            serialin >> SumSpacingsSq; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted SumSpacings" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DimEstimate:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'DimEstimate:' token" << std::endl;
                break;
            }
            
            serialin >> DimEstimate; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted DimEstimate" << std::endl;
                break;
            }
            
            serialin >> token; if (!serialin.good() || token != string("DimEstimateEsd:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'DimEstimateEsd:' token" << std::endl;
                break;
            }
            
            serialin >> DimEstimateEsd; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted DimEstimateEsd" << std::endl;
                break;
            }
            
#ifdef CNEARTREE_INSTRUMENTED
            serialin >> token; if (!serialin.good() || token != string("NodeVisits:")) {
                cout << filenames[choiceAlgorithm] << " badly formatted, no 'NodeVisits:' token" << std::endl;
                break;
            }
            
            serialin >> NodeVisits; if (!serialin.good()) {
                cout << filenames[choiceAlgorithm] << " badly formatted NodeVisits << std::endl;
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
    
	for (int i = 0; i < NUM_ROWS; i++)
	{
        Cell rawcell(cellDArray[i][0], cellDArray[i][1], cellDArray[i][2], cellDArray[i][3], cellDArray[i][4], cellDArray[i][5]);
        mc = rawcell.LatSymMat66((spaceArray[i][0]).substr(0,1));
        primcell = mc*(rawcell.Cell2V6());
        if (!Reducer::Reduce(primcell,m,redprimcell,0.0)){
            cout << "Reduction failed for "<<idArray[i][0]<<" "<<
            cellDArray[i][3]<<" "<<
            cellDArray[i][4]<<" "<<
            cellDArray[i][5]<<" "<<
            cellDArray[i][0]<<" "<<
            cellDArray[i][1]<<" "<<
            cellDArray[i][2]<<" "<<
            spaceArray[i][0] << std::endl;
            cout << "Primitive G6 " << primcell[0]<<" "<<
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
        serialout << "Algorithm: "<< choiceAlgorithm << std::endl;
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
        cout << "unable to save database checkpoint file "<<filenames[choiceAlgorithm] << endl;
    }
}

void findNearest()
{
    CNearTree <unitcell>::iterator nnresult;
	cout << std::endl;
	// unitcell unknownCell = unitcell(probeArray[0], probeArray[1], probeArray[2], probeArray[3], probeArray[4], probeArray[5], 0, 0, 0, 0, 0, 0, 0);
    unitcell unknownCell = unitcell(primredprobe[0], primredprobe[1], primredprobe[2], primredprobe[3], primredprobe[4], primredprobe[5], 0, 0, 0, 0, 0, 0, 0);
	cout << "Raw Unknown Cell\n" <<
        "A: " << probeArray[0] << " " <<
        "B: " << probeArray[1] << " " <<
        "C: " << probeArray[2] << " " <<
		"Alpha: " <<probeArray[3] << " " <<
        "Beta: " << probeArray[4] << " " <<
        "Gamma: " << probeArray[5] <<
        " Lattice: "<< probelattice <<  std::endl;
	cout << "Reduced Primitive Cell\n" <<
        "A: " << primredprobe[0] << " " <<
        "B: " << primredprobe[1] << " " <<
        "C: " << primredprobe[2] << " " <<
        "Alpha: " <<primredprobe[3] << " " <<
        "Beta: " << primredprobe[4] << " " <<
        "Gamma: " << primredprobe[5]  <<  std::endl;
    nnresult = cellTree[choiceAlgorithm-1]->NearestNeighbor(1.e38, unknownCell);
    if (nnresult != cellTree_itend[choiceAlgorithm-1]) {
	unitcell nearestData = *nnresult;
    
    cout << "Depth: " << cellTree[choiceAlgorithm-1]->GetDepth() << std::endl;

	
	numRow = (int)nearestData.getRow();
	cout << "\nNearest Results\n" << "PDBID: " << idArray[numRow][0] << " " <<
        "A: " << cellDArray[numRow][0] << " " <<
        "B: " << cellDArray[numRow][1] << " " <<
        "C: " << cellDArray[numRow][2] << " " <<
        "Alpha: " << cellDArray[numRow][3] << " " <<
        "Beta: "  << cellDArray[numRow][4] << " " <<
        "Gamma: " << cellDArray[numRow][5] << " " <<
        "Space Group: " << spaceArray[numRow][0] << " " <<
        "Z: " << zArray[numRow][0] << std::endl;
    cout << "As Primitive Reduced\n"<<
		"A: " << nearestData.getData(0) << " " <<
        "B: " << nearestData.getData(1) << " " <<
        "C: " << nearestData.getData(2) << " " <<
		"Alpha: " << nearestData.getData(3) << " " <<
        "Beta: " << nearestData.getData(4) << " " <<
        "Gamma: " << nearestData.getData(5) << std::endl;
    }
    	
}

void findSphere()
{
	cout << std::endl;
	// unitcell unknownCell = unitcell(probeArray[0], probeArray[1], probeArray[2], probeArray[3], probeArray[4], probeArray[5], 0, 0, 0, 0, 0, 0, 0);
    unitcell unknownCell = unitcell(primredprobe[0], primredprobe[1], primredprobe[2], primredprobe[3], primredprobe[4], primredprobe[5], 0, 0, 0, 0, 0, 0, 0);
	cout << "Raw Unknown Cell\n" <<
        "A: " << probeArray[0] << " " <<
        "B: " << probeArray[1] << " " <<
        "C: " << probeArray[2] << " " <<
		"Alpha: " <<probeArray[3] << " " <<
        "Beta: " << probeArray[4] << " " <<
        "Gamma: " << probeArray[5] <<
        " Lattice: "<< probelattice <<  std::endl;
	cout << "Reduced Primitive Cell\n" <<
        "A: " << primredprobe[0] << " " <<
        "B: " << primredprobe[1] << " " <<
        "C: " << primredprobe[2] << " " <<
        "Alpha: " <<primredprobe[3] << " " <<
        "Beta: " << primredprobe[4] << " " <<
        "Gamma: " << primredprobe[5]  <<  std::endl;
	vector <unitcell> myvector;
	long sphereData = cellTree[choiceAlgorithm-1]->FindInSphere(sphereRange, myvector, unknownCell);

	cout << "\nSphere Results" << std::endl;
	for (vector <unitcell>::iterator cell = myvector.begin(); cell != myvector.end(); ++cell)
	{
		numRow = (int)(*cell).getRow();
		cout << "PDBID: " << idArray[numRow][0] << " " <<
        "A: " << cellDArray[numRow][0] << " " <<
        "B: " << cellDArray[numRow][1] << " " <<
        "C: " << cellDArray[numRow][2] << " " <<
        "Alpha: " << cellDArray[numRow][3] << " " <<
        "Beta: "  << cellDArray[numRow][4] << " " <<
        "Gamma: " << cellDArray[numRow][5] << " " <<
        "Space Group: " << spaceArray[numRow][0] << " " <<
        "Z: " << zArray[numRow][0] << std::endl;
		cout << "As Primitive Reduced\n"<<
		"A: " << (*cell).getData(0) << " " <<
        "B: " << (*cell).getData(1) << " " <<
        "C: " << (*cell).getData(2) << " " <<
		"Alpha: " << (*cell).getData(3) << " " <<
        "Beta: " << (*cell).getData(4) << " " <<
        "Gamma: " << (*cell).getData(5) << std::endl;
	}
}

void findRange()
{
	cout << std::endl;
	cout << "Unknown Cell\n" << "A: " << probeArray[0] << " " << "B: " << probeArray[1] << " " << "C: " << probeArray[2] << " " <<
		"Alpha: " <<probeArray[3] << " " << "Beta: " << probeArray[4] << " " << "Gamma: " << probeArray[5] << std::endl;
	cout << "\nRange Results\n";
	for (int i = 0; i < NUM_ROWS; i++)
	{
		if ((probeArray[0] + numRangeA) >= cellDArray[i][0] && (probeArray[0] - numRangeA) <= cellDArray[i][0] &&
			(probeArray[1] + numRangeB) >= cellDArray[i][1] && (probeArray[1] - numRangeB) <= cellDArray[i][1] &&
			(probeArray[2] + numRangeC) >= cellDArray[i][2] && (probeArray[2] - numRangeC) <= cellDArray[i][2] &&
			(probeArray[3] + numRangeAlpha) >= cellDArray[i][3] && (probeArray[3] - numRangeAlpha) <= cellDArray[i][3] &&
			(probeArray[4] + numRangeBeta) >= cellDArray[i][4] && (probeArray[4] - numRangeBeta) <= cellDArray[i][4] &&
			(probeArray[5] + numRangeGamma) >= cellDArray[i][5] && (probeArray[5] - numRangeGamma) <= cellDArray[i][5])
		{
			cout << "PDBID: " << idArray[i][0] << " " << 
				"A: " << cellDArray[i][0] << " " << "B: " << cellDArray[i][1] << " " << "C: " << cellDArray[i][2] << " " <<
				"Alpha: " << cellDArray[i][3] << " " << "Beta: " << cellDArray[i][4] << " " << "Gamma: " << cellDArray[i][5] << " " << 
				"Space Group: " << spaceArray[i][0] << " " << "Z: " << zArray[i][0] << std::endl;
		}
	}
}

int main ()
{
	//Create Database
    filenames[0] = "PDBcelldatabase.csv";
    filenames[1] = "PDBcellneartreeL1.dmp";
    filenames[2] = "PDBcellneartreeL2.dmp";
    filenames[3] = "PDBcellneartreeNCDist.dmp";
    filenames[4] = "PDBcellneartreeV7.dmp";
	makeDatabase(filenames[0]);

	unitcell cell;
    
    cout << "sauc Copyright (C) Keith McGill 2013" << std::endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY" << std::endl;
    cout << "This is free software, and you are welcome to" << std::endl;
    cout << "redistribute it under the GPL" << std::endl;
    cout << "See the program documentation for details" << std::endl;

	while (endProgram != 1)
	{
		if (goBack != 1)
		{
			cout << "Search of Alternate Unit Cells\n";
			cout << "\nPlease Input Your Data\n";
            cout << "Lattice Centering (P, A, B, C, F, I, R, H, V): ";
            cin >> probelattice;
			cout << "A: ";
			cin >> probeArray[0];
			cout << "B: ";
			cin >> probeArray[1];
			cout << "C: ";
			cin >> probeArray[2];
			cout << "Alpha: ";
			cin >> probeArray[3];
			cout << "Beta: ";
			cin >> probeArray[4];
			cout << "Gamma: ";
			cin >> probeArray[5];
			cout << std::endl;
            makeprimredprobe();
		}

		while (quitAlgorithm != 1 && endProgram != 1)
		{
			goBack = 0;
			cout << "1. L1";
			cout << "\n2. L2";
			cout << "\n3. NCDist";
			cout << "\n4. V7";
			cout << "\n5. Quit";
			cout << "\nChoose An Algorithm: ";
            priorAlgorithm = choiceAlgorithm;
			cin >> choiceAlgorithm;
        
			if (choiceAlgorithm == 1 && priorAlgorithm!= 1)
			{
				cell.changeOperator(1);
				cell.changeAlgorithm(1);
				quitAlgorithm = 1;
				//Build Tree
				if (!cellTree[choiceAlgorithm-1]) buildNearTree();
			}
			else if (choiceAlgorithm == 2 && priorAlgorithm!= 2)
			{
				cell.changeOperator(1);
				cell.changeAlgorithm(2);
				quitAlgorithm = 1;
				//Build Tree
				if (!cellTree[choiceAlgorithm-1]) buildNearTree();
			}
			else if (choiceAlgorithm == 3 && priorAlgorithm!= 3)
			{
				cell.changeOperator(2);
				cell.changeAlgorithm(3);
				quitAlgorithm = 1;
				//Build Tree
				if (!cellTree[choiceAlgorithm-1]) buildNearTree();
			}
			else if (choiceAlgorithm == 4 && priorAlgorithm!= 4)
			{
				cell.changeOperator(2);
				cell.changeAlgorithm(4);
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
				cout << "Incorrect Choice. Please Choose Again.\n";
                choiceAlgorithm = priorAlgorithm;
			}
            else
            {
                quitAlgorithm = 1;
            }
			cout << std::endl;
		}
		quitAlgorithm = 0;

		while (quitSimilar != 1 && endProgram != 1)
		{
			cout << "1. Nearest";
			cout << "\n2. Sphere";
			cout << "\n3. Range(Does not use any Algorithms)";
			cout << "\n4. Back";
			cout << "\n5. Quit";
			cout << "\nChoose A Similarity: ";
			cin >> choiceSimilar;
			if (choiceSimilar == 1)
			{
				findNearest();
				quitSimilar = 1;
			}
			else if (choiceSimilar == 2)
			{
				cout << "\nPlease Input Your Sphere's Range: ";
				cin >> sphereRange;
				findSphere();
				quitSimilar = 1;
			}
			else if (choiceSimilar == 3)
			{
				cout << "\nPlease Input Your Ranges\n";
				cout << "A: ";
				cin >> numRangeA;
				cout << "B: ";
				cin >> numRangeB;
				cout << "C: ";
				cin >> numRangeC;
				cout << "Alpha: ";
				cin >> numRangeAlpha;
				cout << "Beta: ";
				cin >> numRangeBeta;
				cout << "Gamma: ";
				cin >> numRangeGamma;
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
				cout << "Inncorrect Choice. Please Choose Again.\n";
			}
			cout << std::endl;
		}
		quitSimilar = 0;

		while (quitContinue == 0)
		{
			cout << "1. Input New Values";
			cout << "\n2. Choose a New Algorithm";
			cout << "\n3. Choose a New Similarity";
			cout << "\n4. Quit";
			cout << "\nChoice: ";
			cin >> choiceContinue;
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
				cout << "Inncorrect Choice. Please Choose Again.\n";
			}
			cout << std::endl;
		}
		quitContinue = 0;
	}

	return 0;
}
