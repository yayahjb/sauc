#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK
#include <armadillo>
#include "Cell.h"
#include "V7.h"
#include "NCDist.h"

double NCDist(double *, double *);

int algorithm, operatorCheck;
class unitcell
{
	double numRow;
	double cellD[12];
 
public:
	unitcell()
	{
        cellD[0] = 0.;
		cellD[1] = 0.;
		cellD[2] = 0.;
		cellD[3] = 0.;
		cellD[4] = 0.;
		cellD[5] = 0.;
		cellD[6] = 0.;
		cellD[7] = 0.;
		cellD[8] = 0.;
		cellD[9] = 0.;
		cellD[10] = 0.;
		cellD[11] = 0.;
		numRow = -1;

		
	}
	unitcell(double val0, double val1, double val2, double val3, double val4, double val5, double val6, double val7, double val8, double val9, double val10, double val11, double val12)
	{
		cellD[0] = val0;
		cellD[1] = val1;
		cellD[2] = val2;
		cellD[3] = val3;
		cellD[4] = val4;
		cellD[5] = val5;
		cellD[6] = val6;
		cellD[7] = val7;
		cellD[8] = val8;
		cellD[9] = val9;
		cellD[10] = val10;
		cellD[11] = val11;
		numRow = val12;
	}
	unitcell(double val[6], double row)
	{
		cellD[0] = val[0];
		cellD[1] = val[1];
		cellD[2] = val[2];
		cellD[3] = val[3];
		cellD[4] = val[4];
		cellD[5] = val[5];
		cellD[6] = 0.;
		cellD[7] = 0.;
		cellD[8] = 0.;
		cellD[9] = 0.;
		cellD[10] = 0.;
		cellD[11] = 0.;
		numRow = row;
	}
    
    void getCell(double cell[6], double * row ) {
        int ii;
        for (ii=0; ii < 6; ii++) {
            cell[ii] = cellD[ii];
        }
        *row = numRow;
    }

	double getData(int index)
	{
		return cellD[index];
	}

	double getRow()
	{
		return numRow;
	}

	void changeAlgorithm(int numAlg)
	{
		algorithm = numAlg;
	}

	void changeOperator(int numOper)
	{
		operatorCheck = numOper;
	}

	unitcell operator- (const unitcell &unitcell2) const
	{
		if (operatorCheck == 1)
		{
		unitcell cellReturn
			(cellD[0] - unitcell2.cellD[0],
			cellD[1] - unitcell2.cellD[1],
			cellD[2] - unitcell2.cellD[2],
			cellD[3] - unitcell2.cellD[3],
			cellD[4] - unitcell2.cellD[4],
			cellD[5] - unitcell2.cellD[5],
			0,
			0,
			0,
			0,
			0,
			0,
			numRow);
		return(cellReturn);
		}

		else if(operatorCheck == 2)
		{
		unitcell cellReturn
			(cellD[0],
			cellD[1],
			cellD[2],
			cellD[3],
			cellD[4],
			cellD[5],
			unitcell2.cellD[0],
			unitcell2.cellD[1],
			unitcell2.cellD[2],
			unitcell2.cellD[3],
			unitcell2.cellD[4],
			unitcell2.cellD[5],
			numRow);
		return(cellReturn);
		}
	}

	double Norm(void) const
	{
		if (algorithm == 1)
		{
			return(std::abs(cellD[0]) + std::abs(cellD[1]) + std::abs(cellD[2]) + std::abs(cellD[3]) + std::abs(cellD[4]) + std::abs(cellD[5]));
		}
		else if (algorithm == 2)
		{
			return(std::sqrt((cellD[0]*cellD[0]) + (cellD[1]*cellD[1]) + (cellD[2]*cellD[2]) +
			(cellD[3]*cellD[3]) + (cellD[4]*cellD[4]) + cellD[5]*cellD[5]));
		}
		else if (algorithm == 3)
		{
            const Cell c1(cellD[0],cellD[1],cellD[2],cellD[3],cellD[4],cellD[5]);
            const Cell c2(cellD[6],cellD[7],cellD[8],cellD[9],cellD[10],cellD[11]);
            const arma::vec6 gv1(c1.Cell2V6());
            const arma::vec6 gv2(c2.Cell2V6());
            double dgv1[6];
            double dgv2[6];
            int ii;
            for (ii=0; ii < 6; ii++){
                dgv1[ii] = gv1[ii];
                dgv2[ii] = gv2[ii];
            }
            return std::sqrt(NCDist(dgv1,dgv2));            
		}
		else if (algorithm == 4)
		{
            const Cell c1(cellD[0],cellD[1],cellD[2],cellD[3],cellD[4],cellD[5]);
            const Cell c2(cellD[6],cellD[7],cellD[8],cellD[9],cellD[10],cellD[11]);
            const V7 gv1(c1.Cell2V6());
            const V7 gv2(c2.Cell2V6());
            
			//V7 dist
            
			return (gv1-gv2).Norm();
		}
		return 0;
	}
};