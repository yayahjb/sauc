#ifndef V7_H
#define V7_H

#include "VecN.h"
#include "V7.h"
#include "G6.h"
#include "Cell.h"

class Cell;
class G6;

#include <ostream>
#include <string>
#include <vector>

class V7: public VecN {
public:
   friend std::ostream& operator<< ( std::ostream&, const V7& );

   V7( void );
   V7( const V7& v );
   explicit V7( const double v[7] );
   explicit V7( const G6& v );
   V7(const Cell& c);
   V7( const VecN& v );
   V7( const std::string& s );
   V7( const std::vector<double>& v );
   ~V7(void) {}

   V7& operator= (const V7& v);
   V7& operator= (const std::string& s);
   V7& operator= (const G6& v);
   V7& operator= (const Cell& v);

   static double DistanceBetween( const V7& v1, const V7& v2 );

};


#endif

