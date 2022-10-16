#ifndef UNITCELL_H
#define UNITCELL_H

class unitcell
{
protected:
    size_t numRow;
    double cellD[6];
    double converted[7];
    int myalgorithm;
    char db;
 
public:
    static int algorithm, operatorCheck;
    static double Scaledist;
    unitcell();
    unitcell(const double val[6], const size_t row);
    unitcell(const double val[6], const double row);
    unitcell(const double val0, const double val1, const double val2, 
             const double val3, const double val4, const double val5,
             const size_t row, const char dbid);
    unitcell(const double val[6], const size_t row, const char dbid);
    unitcell(const double mycell[6], const char latsym, 
             const size_t row, const char dbid, const int alg);
    void getCell(double cell[6], double * row );
    void getCell(double cell[6], double * row, char * dbid );
    double getData(const int index);
    size_t getRow();
    char getDBID();
    void changeAlgorithm(int numAlg);
    int getConverted(double conv[7] );
    int setConverted( const int alg );
    int setConverted( void );
    void changeOperator(int numOper);
    void changeScaledist(double scaledist);
    double DistanceBetween(unitcell u, unitcell v);
    double distance_from(unitcell v);
};
#endif
