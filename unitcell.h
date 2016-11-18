#define ARMA_DONT_USE_BLAS
#define ARMA_DONT_USE_LAPACK
#include <armadillo>
#include "Cell.h"
#include "V7.h"
#include "NCDist.h"

double NCDist(double *, double *);
static double pi = 3.141592653589793;
static double torad = pi/180.;

int algorithm, operatorCheck;
double Scaledist = 1.;

class unitcell
{
	size_t numRow;
	double cellD[12];
    char db;
 
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
        db = 'p';

	}
	unitcell(const double a1, const double b1, const double c1, const double alpha1,
                 const double beta1, const double gamma1, const double a2, const double b2,
                 const double c2, const double alpha2, const double beta2, const double gamma2,
                 const size_t row)
	{
		cellD[0] = a1;
		cellD[1] = b1;
		cellD[2] = c1;
		cellD[3] = alpha1;
		cellD[4] = beta1;
		cellD[5] = gamma1;
		cellD[6] = a2;
		cellD[7] = b2;
		cellD[8] = c2;
		cellD[9] = alpha2;
		cellD[10] = beta2;
		cellD[11] = gamma2;
		numRow = row;
        db = 'p';
	}
    unitcell(const double a1, const double b1, const double c1, const double alpha1,
             const double beta1, const double gamma1, const double a2, const double b2,
             const double c2, const double alpha2, const double beta2, const double gamma2,
             const size_t row, const char dbid)
    {
        cellD[0] = a1;
        cellD[1] = b1;
        cellD[2] = c1;
        cellD[3] = alpha1;
        cellD[4] = beta1;
        cellD[5] = gamma1;
        cellD[6] = a2;
        cellD[7] = b2;
        cellD[8] = c2;
        cellD[9] = alpha2;
        cellD[10] = beta2;
        cellD[11] = gamma2;
        numRow = row;
        db = dbid;
    }
	unitcell(const double val[6], const size_t row)
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
        db = 'p';
	}
	unitcell(const double val[6], const double row)
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
        numRow = (size_t)(row+.5);
        db = 'p';
    }

    unitcell(const double val[6], const size_t row, const char dbid)
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
        db = dbid;
	}
    
    /* deprecated version of getCell from before introduction of db is*/
    void getCell(double cell[6], double * row ) {
        int ii;
        for (ii=0; ii < 6; ii++) {
            cell[ii] = cellD[ii];
        }
        *row = numRow;
    }


    void getCell(double cell[6], double * row, char * dbid ) {
        int ii;
        for (ii=0; ii < 6; ii++) {
            cell[ii] = cellD[ii];
        }
        *row = numRow;
        *dbid = db;
    }

	double getData(const int index) const
	{
		return cellD[index];
	}

	size_t getRow() const
	{
		return numRow;
	}

    char getDBID() const
    {
        return db;
    }


	void changeAlgorithm(int numAlg) const
	{
		algorithm = numAlg;
	}

	void changeOperator(int numOper) const
	{
		operatorCheck = numOper;
	}

	void changeScaledist(double scaledist) const
	{
		Scaledist = scaledist;
	}
    
	unitcell operator- (const unitcell &unitcell2) const
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

	double Norm(void) const
	{
		if (algorithm == 1)
		{
			return (std::abs(cellD[0]-cellD[6]) +
                   std::abs(cellD[1]-cellD[7]) +
                   std::abs(cellD[2]-cellD[8]) +
                   std::abs(cellD[3]*(cellD[1]+cellD[2])/2.-cellD[9]*(cellD[7]+cellD[8])/2.)*torad +
                   std::abs(cellD[4]*(cellD[0]+cellD[2])/2.-cellD[10]*(cellD[6]+cellD[8])/2.)*torad +
                   std::abs(cellD[5]*(cellD[0]+cellD[1])/2.-cellD[11]*(cellD[6]+cellD[7])/2.)*torad)*Scaledist;
		}
		else if (algorithm == 2)
		{
			return (std::sqrt((cellD[0]-cellD[6])*(cellD[0]-cellD[6]) +
                    (cellD[1]-cellD[7])*(cellD[1]-cellD[7]) +
                    (cellD[2]-cellD[8])*(cellD[2]-cellD[8]) +
			        (cellD[3]*(cellD[1]+cellD[2])/2.-cellD[9]*(cellD[7]+cellD[8])/2.)*torad*(cellD[3]*(cellD[1]+cellD[2])/2.-cellD[9]*(cellD[7]+cellD[8])/2.)*torad +
                    (cellD[4]*(cellD[0]+cellD[2])/2.-cellD[10]*(cellD[6]+cellD[8])/2.)*torad*(cellD[4]*(cellD[0]+cellD[2])/2.-cellD[10]*(cellD[6]+cellD[8])/2.)*torad +
                    (cellD[5]*(cellD[0]+cellD[1])/2.-cellD[11]*(cellD[6]+cellD[7])/2.)*torad*(cellD[5]*(cellD[0]+cellD[1])/2.-cellD[11]*(cellD[6]+cellD[7])/2.)*torad))*Scaledist;
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
            return std::sqrt(NCDist(dgv1,dgv2))*Scaledist;
		}
		else if (algorithm == 4)
		{
            const Cell c1(cellD[0],cellD[1],cellD[2],cellD[3],cellD[4],cellD[5]);
            const Cell c2(cellD[6],cellD[7],cellD[8],cellD[9],cellD[10],cellD[11]);
            const V7 gv1(c1.Cell2V6());
            const V7 gv2(c2.Cell2V6());
            
			//V7 dist
            
			return ((gv1-gv2).Norm())*Scaledist;
		}
		return 0;
	}
};
