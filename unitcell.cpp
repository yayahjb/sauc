#include "Cell.h"
#include "V7.h"
#include "cdistances.h"
#include "S6M_SellingReduce.h"
#include "unitcell.h"
#include <cmath>
#include <cfloat>

#define local_isnan(x) ((x)!=(x)?1:0) 
#define local_isinf(x) ((x)>DBL_MAX||(x)<(-DBL_MAX)?1:0) 

    static double pi = 3.141592653589793;
    static double torad = pi/180.;

    int unitcell::algorithm, unitcell::operatorCheck;
    double unitcell::Scaledist;

    unitcell::unitcell()
    {
      cellD[0] = 0.;
      cellD[1] = 0.;
      cellD[2] = 0.;
      cellD[3] = 0.;
      cellD[4] = 0.;
      cellD[5] = 0.;
      numRow = -1;
      db = 'p';
      int ii;
      for (ii=0;ii<12;ii++){
        if (local_isnan(cellD[ii])||local_isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2]
                   << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5] 
                   <<  "]"<< std::endl;
          break; 
        }
      }
      myalgorithm=0;
      converted[0]=converted[1]=converted[2]=converted[3]=converted[4]=converted[5]=converted[6]=0;
    }

    unitcell::unitcell(const double val[6], const size_t row)
    {
      cellD[0] = val[0];
      cellD[1] = val[1];
      cellD[2] = val[2];
      cellD[3] = val[3];
      cellD[4] = val[4];
      cellD[5] = val[5];
      numRow = row;
      db = 'p';
      int ii;
      for (ii=0;ii<6;ii++){
        if (local_isnan(cellD[ii])||local_isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2] 
                   << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5]
                   << "]"<< std::endl;
          break;
        }
      }
      myalgorithm=0;
      converted[0]=converted[1]=converted[2]=converted[3]=converted[4]=converted[5]=converted[6]=0;
    }

    unitcell::unitcell(const double val[6], const double row)
    {
      cellD[0] = val[0];
      cellD[1] = val[1];
      cellD[2] = val[2];
      cellD[3] = val[3];
      cellD[4] = val[4];
      cellD[5] = val[5];
      numRow = (size_t)(row+.5);
      db = 'p';
      int ii;
      for (ii=0;ii<6;ii++){
        if (local_isnan(cellD[ii])||local_isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2]
                    << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5]
                    << "]"<< std::endl;
          break;
        }
      }
      myalgorithm=0;
      converted[0]=converted[1]=converted[2]=converted[3]=converted[4]=converted[5]=converted[6]=0;
 }

    unitcell::unitcell(const double val0, const double val1, const double val2, 
             const double val3, const double val4, const double val5,
             const size_t row, const char dbid)
    {
      cellD[0] = val0;
      cellD[1] = val1;
      cellD[2] = val2;
      cellD[3] = val3;
      cellD[4] = val4;
      cellD[5] = val5;
      numRow = row;
      db = dbid;
      int ii;
      for (ii=0;ii<6;ii++){
        if (local_isnan(cellD[ii])||local_isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2]
                    << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5]
                    << "]"<< std::endl;
        }
      }
      myalgorithm=0;
      converted[0]=converted[1]=converted[2]=converted[3]=converted[4]=converted[5]=converted[6]=0;
    }
    
    unitcell::unitcell(const double val[6], const size_t row, const char dbid)
    {
      cellD[0] = val[0];
      cellD[1] = val[1];
      cellD[2] = val[2];
      cellD[3] = val[3];
      cellD[4] = val[4];
      cellD[5] = val[5];
      numRow = row;
      db = dbid;
      int ii;
      for (ii=0;ii<6;ii++){
        if (local_isnan(cellD[ii])||local_isinf(cellD[ii])){
          std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2]
                    << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5]
                    << "]"<< std::endl;
        }
      }
      myalgorithm=0;
      converted[0]=converted[1]=converted[2]=converted[3]=converted[4]=converted[5]=converted[6]=0;
    }

    unitcell::unitcell(const double mycell[6], const char latsym, const size_t row, const char dbid, const int alg)
    {
      G6 g6vec;
      double primcell[6];
      double redprimcell[6];
      double dmc[36];
      double d7cell[7];
      double d7red[7];
      double s6cell[6];
      double s6red[6];
      int reduced;
      int ii;
      myalgorithm = alg;
      CS6M_CelldegtoG6(mycell,g6vec);
      CS6M_LatSymMat66(g6vec,latsym,dmc,primcell);
      switch (myalgorithm) {
        case 1:  /* L1 */
        case 2:  /* L2 */
        case 3:  /* NCDist */
        case 4:  /* V7 */
        case 7:  /* DC7unsrt */
             CS6M_G6Reduce(primcell,redprimcell,reduced);
             break;
        case 5:  /* D7Dist */
             CS6M_G6toD7(primcell,d7cell);
             CS6M_D7Reduce(d7cell,d7red,reduced);
             CS6M_D7toG6(d7red,redprimcell);
             break;
        case 6:  /* S6Dist */
        default: /* and anything else */
             CS6M_G6toS6(primcell,s6cell);
             CS6M_S6Reduce(s6cell,s6red,reduced);
             CS6M_S6toG6(s6red,redprimcell);
             break;
      }
      CS6M_G6toCelldeg(redprimcell,cellD);
      converted[6]=0.;

      switch (myalgorithm) {
        case 1:  /* L1 */
        case 2:  /* L2 */
             converted[0]=cellD[0];
             converted[1]=cellD[1];
             converted[2]=cellD[2];
             converted[3]=torad*cellD[3]*(cellD[1]+cellD[2])/2.;
             converted[4]=torad*cellD[4]*(cellD[0]+cellD[2])/2.;
             converted[5]=torad*cellD[5]*(cellD[0]+cellD[1])/2.;
             break;
        case 3:  /* NCDist */
             for (ii=0; ii<6; ii++) converted[ii]=redprimcell[ii];
             break;
        case 4:  /* V7 */
             CS6M_G6toV7(redprimcell,converted);
             break; 
        case 5:  /* D7Dist */
             CS6M_G6toD7(redprimcell,converted);
             break;
        case 6:  /* S6Dist */
             CS6M_G6toS6(redprimcell,converted);
             break;
        case 7:  /* DC7unsrt */
             CS6M_G6toDC7unsorted(redprimcell,converted);
             break;
        default:
             CS6M_G6toS6(redprimcell,converted);
             break;
      }
      numRow =  row;
      db = dbid;

    }

    
    /* deprecated version of getCell from before introduction of dbid*/
    void unitcell::getCell(double cell[6], double * row ) {
    int ii;
    for (ii=0; ii < 6; ii++) {
        cell[ii] = cellD[ii];
    }
    *row = numRow;
    }


    void unitcell::getCell(double cell[6], double * row, char * dbid ) {
    int ii;
    for (ii=0; ii < 6; ii++) {
        cell[ii] = cellD[ii];
    }
    *row = numRow;
    *dbid = db;
    }

    double unitcell::getData(const int index) 
    {
        return cellD[index];
    }

    size_t unitcell::getRow()
    {
        return numRow;
    }

    char unitcell::getDBID()
    {
    return db;
    }


    void unitcell::changeAlgorithm(int numAlg)
    {
        algorithm = numAlg;
    }

    int unitcell::getConverted(double conv[7] ) {
    int ii;
    for (ii=0; ii < 7; ii++) {
        conv[ii] = converted[ii];
    }
    return myalgorithm;
    }

    int unitcell::setConverted( const int alg ) {
      double primcell[6];
      double redprimcell[6];
      double d7cell[7];
      double d7red[7];
      double s6cell[6];
      double s6red[6];
      int reduced, ii;
      if (alg == myalgorithm) return  myalgorithm;
      myalgorithm=alg;
      CS6M_CelldegtoG6(cellD,primcell);
      switch (myalgorithm) {
        case 1:  /* L1 */
        case 2:  /* L2 */
        case 3:  /* NCDist */
        case 4:  /* V7 */
        case 7:  /* DC7unsrt */
             CS6M_G6Reduce(primcell,redprimcell,reduced);
             if (!reduced) {
               myalgorithm=0;
               for(ii=0; ii<7; ii++) converted[ii]=0.;
               return 0;
             }
             break;
        case 5:  /* D7Dist */
             CS6M_G6toD7(primcell,d7cell);
             CS6M_D7Reduce(d7cell,d7red,reduced);
             CS6M_D7toG6(d7red,redprimcell);
             if (!reduced) {
               myalgorithm=0;
               for(ii=0; ii<7; ii++) converted[ii]=0.;
               return 0;
             }
             break;
        case 6:  /* S6Dist */
        default: /* and anything else */
             CS6M_G6toS6(primcell,s6cell);
             CS6M_S6Reduce(s6cell,s6red,reduced);
             if (!reduced) {
               myalgorithm=0;
               for(ii=0; ii<7; ii++) converted[ii]=0.;
               return 0;
             }
             CS6M_S6toG6(s6red,redprimcell);
             break;
      }
      CS6M_G6toCelldeg(redprimcell,cellD);
      converted[6]=0.;

      switch (myalgorithm) {
        case 1:  /* L1 */
        case 2:  /* L2 */
             converted[0]=cellD[0];
             converted[1]=cellD[1];
             converted[2]=cellD[2];
             converted[3]=torad*cellD[3]*(cellD[1]+cellD[2])/2.;
             converted[4]=torad*cellD[4]*(cellD[0]+cellD[2])/2.;
             converted[5]=torad*cellD[5]*(cellD[0]+cellD[1])/2.;
             break;
        case 3:  /* NCDist */
             for (ii=0; ii<6; ii++) converted[ii]=redprimcell[ii];
             break;
        case 4:  /* V7 */
             CS6M_G6toV7(redprimcell,converted);
             break; 
        case 5:  /* D7Dist */
             CS6M_G6toD7(redprimcell,converted);
             break;
        case 6:  /* S6Dist */
             CS6M_G6toS6(redprimcell,converted);
             break;
        case 7:  /* DC7unsrt */
             CS6M_G6toDC7unsorted(redprimcell,converted);
             break;
        default:
             CS6M_G6toS6(redprimcell,converted);
             break;
      }
      return myalgorithm;
    }

    int unitcell::setConverted(void) {
      int myalg;
      myalg=unitcell::algorithm;
      return this->setConverted(myalg);
    }

    void unitcell::changeOperator(int numOper)
    {
        operatorCheck = numOper;
    }

    void unitcell::changeScaledist(double scaledist)
    {
        Scaledist = scaledist;
    }
    
    double unitcell::DistanceBetween(unitcell u, unitcell v)
    {
        return u.distance_from(v);
    }

    double unitcell::distance_from(unitcell v)
    {
        double df;
        this->setConverted(algorithm);
        v.setConverted(algorithm);

        switch (algorithm) {

        case 1: /* L1 */
            {
               df = (fabs(converted[0]-v.converted[0])
                 + fabs(converted[1]-v.converted[1]) 
                 + fabs(converted[2]-v.converted[2]) 
                 + fabs(converted[3]-v.converted[3]) 
                 + fabs(converted[4]-v.converted[4]) 
                 + fabs(converted[5]-v.converted[5]))*Scaledist;
               if (local_isinf(df) || local_isnan(df)  || df > DBL_MAX || df < -DBL_MAX) {
                   df=DBL_MAX;
               }
               return df;
            }
            break;

        case 2: /* L2 */
            {
               df = std::sqrt((converted[0]-v.converted[0])*(converted[0]-v.converted[0])
                           +  (converted[1]-v.converted[1])*(converted[1]-v.converted[1])
                           +  (converted[2]-v.converted[2])*(converted[2]-v.converted[2])
                           +  (converted[3]-v.converted[3])*(converted[3]-v.converted[3])
                           +  (converted[4]-v.converted[4])*(converted[4]-v.converted[4])
                           +  (converted[5]-v.converted[5])*(converted[5]-v.converted[5]))
                     *Scaledist;
               if (local_isinf(df) || local_isnan(df)  || df > DBL_MAX || df < -DBL_MAX) {
                   df=DBL_MAX;
               }
               return df;

            }
            break;

        case 3: /* NCDist */
            {
                df = std::sqrt(NCDist(converted,v.converted))*Scaledist;
                if (local_isinf(df) || local_isnan(df)  || df > DBL_MAX || df < -DBL_MAX) {
                    df=DBL_MAX;
                }
                return df;
            }
            break;

        case 4: /* V7 */
            {
                df = std::sqrt((converted[0]-v.converted[0])*(converted[0]-v.converted[0])
                         +(converted[1]-v.converted[1])*(converted[1]-v.converted[1])
                         +(converted[2]-v.converted[2])*(converted[2]-v.converted[2])
                         +(converted[3]-v.converted[3])*(converted[3]-v.converted[3])
                         +(converted[4]-v.converted[4])*(converted[4]-v.converted[4])
                         +(converted[5]-v.converted[5])*(converted[5]-v.converted[5])
                         +(converted[6]-v.converted[6])*(converted[6]-v.converted[6]))*Scaledist;
                if (local_isinf(df) || local_isnan(df)) {
                  df=DBL_MAX;
                  std::cerr<<" numRow: "<< numRow<< std::endl;
                  std::cerr<< "cellD: [" << cellD[0] <<", "<< cellD[1] <<","<< cellD[2] 
                           <<","<< cellD[3] <<","<< cellD[4]<<","<< cellD[5]  <<"]"<<std::endl; 
                  std::cerr<< "v.cellD: [" << v.cellD[0] <<", "<< v.cellD[1] <<","<< v.cellD[2] 
                           <<","<< v.cellD[3] <<","<< v.cellD[4]<<","<< v.cellD[5]  <<","<<std::endl; 
                  std::cerr<< "converted: [" << converted[0] <<", "<< converted[1] <<","<< converted[2]
                           <<","<< converted[3] <<","<< converted[4]<<","<< converted[5]  <<","
                           << converted[6] <<"]"<<std::endl; 
                  std::cerr<< "v.converted: [" << v.converted[0] <<", "<< v.converted[1] <<","<< v.converted[2]
                           <<","<< v.converted[3] <<","<< v.converted[4]<<","<< v.converted[5]  <<","
                           << v.converted[6] <<"]"<<std::endl; 
                  int ii;
                  for (ii=0;ii<6;ii++){
                    if (local_isnan(cellD[ii])||local_isinf(cellD[ii])||local_isnan(v.cellD[ii])||local_isinf(v.cellD[ii])){
                      std::cerr<< "cellD: [" << cellD[0] << ", "<< cellD[1] << ", "<< cellD[2]
                               << ", "<< cellD[3] << ", "<< cellD[4] << ", "<< cellD[5]
                               << "], v: ["<< v.cellD[0] << ", "<< v.cellD[1] << ", "<< v.cellD[2]
                               << ", "<< v.cellD[3] << ", "<< v.cellD[4] << ", "<< v.cellD[5] 
                               << "]"<< db <<std::endl;
                      break;
                    }
                  }
                }
                return df;
            }
            break;

        case 5: /* D7Dist */
            {
                df = std::sqrt(D7Dist(converted,v.converted))*Scaledist;
                if (local_isinf(df) || local_isnan(df)  || df > DBL_MAX || df < -DBL_MAX) {
                    df=DBL_MAX;
                }
                return df;

            }
            break;

        case 6: /* S6Dist */
            {
                df = std::sqrt(CS6Dist(converted,v.converted))*Scaledist;
                if (local_isinf(df) || local_isnan(df)  || df > DBL_MAX || df < -DBL_MAX) {
                   df=DBL_MAX;
                }
                return df;

            }
            break;

        case 7: /* DC7unsrt */
            {
                df = std::sqrt(DC7unsortedsqDist(converted,v.converted))*Scaledist;
                if (local_isinf(df) || local_isnan(df)  || df > DBL_MAX || df < -DBL_MAX) {
                   df=DBL_MAX;
                }
                return df;

            }

        default:
            return 0;
            break;

        }
    }
