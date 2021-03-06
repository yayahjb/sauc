#include "Cell.h"
#include "V7.h"
#include "NCDist.h"
#include "D7Dist.h"
#include "CS6Dist.h"
#include "S6M_SellingReduce.h"
#include <cmath>

#define isnan(x) ((x)!=(x))
#define isinf(x) ((x)>DBL_MAX||(x)<(-DBL_MAX))

double NCDist(double *, double *);
double D7Dist(double *, double *);
double CS6Dist(double *, double *);
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
      int ii;
      for (ii=0;ii<12;ii++){
        if (isnan(cellD[ii])||isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] << 
                           ", "<< cellD[6] << ", "<< cellD[7] << ", "<< cellD[8] << ", "<< cellD[9] << ", "<< cellD[10] << ", "<< cellD[11] << "]"<< std::endl; 
        }
      }
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
      int ii;
      for (ii=0;ii<12;ii++){
        if (isnan(cellD[ii])||isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] << 
                           ", "<< cellD[6] << ", "<< cellD[7] << ", "<< cellD[8] << ", "<< cellD[9] << ", "<< cellD[10] << ", "<< cellD[11] << "]"<< std::endl; 
        }
      }
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
      int ii;
      for (ii=0;ii<12;ii++){
        if (isnan(cellD[ii])||isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] << 
                           ", "<< cellD[6] << ", "<< cellD[7] << ", "<< cellD[8] << ", "<< cellD[9] << ", "<< cellD[10] << ", "<< cellD[11] << "]"<< std::endl; 
        }
      }
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
      int ii;
      for (ii=0;ii<12;ii++){
        if (isnan(cellD[ii])||isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] <<
                           ", "<< cellD[6] << ", "<< cellD[7] << ", "<< cellD[8] << ", "<< cellD[9] << ", "<< cellD[10] << ", "<< cellD[11] << "]"<< std::endl;
        }
      }
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
      int ii;
      for (ii=0;ii<12;ii++){
        if (isnan(cellD[ii])||isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] <<
                           ", "<< cellD[6] << ", "<< cellD[7] << ", "<< cellD[8] << ", "<< cellD[9] << ", "<< cellD[10] << ", "<< cellD[11] << "]"<< std::endl;
        }
      }
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
      int ii;
      for (ii=0;ii<12;ii++){
        if (isnan(cellD[ii])||isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] <<
                           ", "<< cellD[6] << ", "<< cellD[7] << ", "<< cellD[8] << ", "<< cellD[9] << ", "<< cellD[10] << ", "<< cellD[11] << "]"<< std::endl;
        }
      }
    }
    
    /* deprecated version of getCell from before introduction of dbid*/
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
        numRow, db);
        return(cellReturn);
    }

    double Norm(void) const
    {
            double dnorm;
        if (algorithm == 1)
        {
               dnorm =  (fabs(cellD[0]-cellD[6]) +
               fabs(cellD[1]-cellD[7]) +
               fabs(cellD[2]-cellD[8]) +
               fabs(cellD[3]*(cellD[1]+cellD[2])/2.-cellD[9]*(cellD[7]+cellD[8])/2.)*torad +
               fabs(cellD[4]*(cellD[0]+cellD[2])/2.-cellD[10]*(cellD[6]+cellD[8])/2.)*torad +
               fabs(cellD[5]*(cellD[0]+cellD[1])/2.-cellD[11]*(cellD[6]+cellD[7])/2.)*torad)*Scaledist;
               if (isinf(dnorm) || isnan(dnorm)  || dnorm > DBL_MAX || dnorm < -DBL_MAX) {
                   dnorm=DBL_MAX;
               }
               return dnorm;
        }
        else if (algorithm == 2)
        {
               dnorm = (std::sqrt(
                      (cellD[0]-cellD[6])*(cellD[0]-cellD[6]) +
                      (cellD[1]-cellD[7])*(cellD[1]-cellD[7]) +
                      (cellD[2]-cellD[8])*(cellD[2]-cellD[8]) +
        	  (cellD[3]*(cellD[1]+cellD[2])/2.-cellD[9]*(cellD[7]+cellD[8])/2.)*torad
                     *(cellD[3]*(cellD[1]+cellD[2])/2.-cellD[9]*(cellD[7]+cellD[8])/2.)*torad +
                      (cellD[4]*(cellD[0]+cellD[2])/2.-cellD[10]*(cellD[6]+cellD[8])/2.)*torad
                     *(cellD[4]*(cellD[0]+cellD[2])/2.-cellD[10]*(cellD[6]+cellD[8])/2.)*torad +
                      (cellD[5]*(cellD[0]+cellD[1])/2.-cellD[11]*(cellD[6]+cellD[7])/2.)*torad
                     *(cellD[5]*(cellD[0]+cellD[1])/2.-cellD[11]*(cellD[6]+cellD[7])/2.)*torad))
                     *Scaledist;
               if (isinf(dnorm) || isnan(dnorm)  || dnorm > DBL_MAX || dnorm < -DBL_MAX) {
                   dnorm=DBL_MAX;
               }
               return dnorm;

        }

        else if (algorithm == 3) /* NCDist */
        {
                double c1[6], c2[6], g1[6], g2[6];
                CS6M_comptovec6(cellD[0],cellD[1],cellD[2],cellD[3],cellD[4],cellD[5],c1);
                CS6M_comptovec6(cellD[6],cellD[7],cellD[8],cellD[9],cellD[10],cellD[11],c2);
                CS6M_CelldegtoG6(c1,g1);
                CS6M_CelldegtoG6(c2,g2);
                dnorm = std::sqrt(NCDist(g1,g2))*Scaledist;
                if (isinf(dnorm) || isnan(dnorm)  || dnorm > DBL_MAX || dnorm < -DBL_MAX) {
                    dnorm=DBL_MAX;
                }
                return dnorm;
        }

        else if (algorithm == 4) /* V7 */
        {
                const Cell c1(cellD[0],cellD[1],cellD[2],cellD[3],cellD[4],cellD[5]);
                const Cell c2(cellD[6],cellD[7],cellD[8],cellD[9],cellD[10],cellD[11]);
                const Cell c1v6(c1.Cell2V6());
                const Cell c2v6(c2.Cell2V6());
                const V7 gv1(c1v6);
                const V7 gv2(c2v6);
                VecN gv3(7);
        
        	//V7 dist

                gv3= VecN(gv1)-VecN(gv2);
        
                dnorm = (gv3.Norm())*Scaledist;
                if (isinf(dnorm) || isnan(dnorm)) {
                  dnorm=DBL_MAX;
                  std::cerr<<" numRow: "<< numRow<< std::endl;
                  std::cerr<< "cellD: [" << cellD[0] <<", "<< cellD[1] <<","<< cellD[2] <<","<< cellD[3] <<","<< cellD[4]<<","<< cellD[5]  <<","<<std::endl; 
                  std::cerr<< "cellD: [" << cellD[6] <<", "<< cellD[7] <<","<< cellD[8] <<","<< cellD[9] <<","<< cellD[10]<<","<< cellD[11]  <<","<<std::endl; 
                  std::cerr<< "c1v6: [" << c1v6[0] <<", "<< c1v6[1] <<","<< c1v6[2] <<","<< c1v6[3] <<","<< c1v6[4]<<","<< c1v6[5]  <<","<<std::endl; 
                  std::cerr<< "c2v6: [" << c2v6[0] <<", "<< c2v6[1] <<","<< c2v6[2] <<","<< c2v6[3] <<","<< c2v6[4]<<","<< c2v6[5]  <<","<<std::endl; 
                  std::cerr<< "gv1: [" << gv1[0] << ", "<< gv1[1] << ", "<< gv1[2] << ", "<< gv1[3] << ", "<< gv1[4] << ", "<< gv1[5] << ", "<< gv1[6] << ']' << std::endl;
                  std::cerr<< "gv2: [" << gv2[0] << ", "<< gv2[2] << ", "<< gv2[2] << ", "<< gv2[3] << ", "<< gv2[4] << ", "<< gv2[5] << ", "<< gv1[5] << ']' << std::endl;
                  std::cerr<< "gv3: [" << gv3[0] << ", "<< gv3[3] << ", "<< gv3[2] << ", "<< gv3[3] << ", "<< gv3[4] << ", "<< gv3[5] << ", "<< gv1[5] << ']' << std::endl;
                  
                  int ii;
                  for (ii=0;ii<12;ii++){
                    if (isnan(cellD[ii])||isinf(cellD[ii])){
                      std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] <<
                             ", "<< cellD[6] << ", "<< cellD[7] << ", "<< cellD[8] << ", "<< cellD[9] << ", "<< cellD[10] << ", "<< cellD[11] << "]"<< db <<std::endl;
                    }
                  }
                }
                return dnorm;

        }

            else if (algorithm == 5) /* D7Dist */
            {
                int red1, red2;
                double c1[6], c2[6], g1[6], g2[6];
                double d1[7], d2[7];

                CS6M_comptovec6(cellD[0],cellD[1],cellD[2],cellD[3],cellD[4],cellD[5],c1);
                CS6M_comptovec6(cellD[6],cellD[7],cellD[8],cellD[9],cellD[10],cellD[11],c2);
                CS6M_CelldegtoG6(c1,g1);
                CS6M_CelldegtoG6(c2,g2);
                CS6M_G6toD7(g1,d1);
                CS6M_G6toD7(g2,d2);
                dnorm = std::sqrt(D7Dist(d1,d2))*Scaledist;
                if (isinf(dnorm) || isnan(dnorm)  || dnorm > DBL_MAX || dnorm < -DBL_MAX) {
                    dnorm=DBL_MAX;
                }
                return dnorm;

            }

            else if (algorithm == 6) /* S6Dist */
            {
                int red1,red2;
                double c1[6], c2[6], g1[6], g2[6];
                double s1[6], s2[6];
                CS6M_comptovec6(cellD[0],cellD[1],cellD[2],cellD[3],cellD[4],cellD[5],c1);
                CS6M_comptovec6(cellD[6],cellD[7],cellD[8],cellD[9],cellD[10],cellD[11],c2);
                CS6M_CelldegtoG6(c1,g1);
                CS6M_CelldegtoG6(c2,g2);
                CS6M_G6toS6(g1,s1);
                CS6M_G6toS6(g2,s2);
                dnorm = std::sqrt(CS6Dist(s1,s2))*Scaledist;
                if (isinf(dnorm) || isnan(dnorm)  || dnorm > DBL_MAX || dnorm < -DBL_MAX) {
                   dnorm=DBL_MAX;
                }
                return dnorm;

            }


        return 0;
    }
};
