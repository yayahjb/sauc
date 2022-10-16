#ifndef DC7unsrt_H
#define DC7unsrt_H

#include "VecN.h"

class Cell;
class D6;
class DeloneTetrahedron;
class G6;

#include <ostream>
#include <string>

class DC7unsrt: public VecN {
public:
   friend std::ostream& operator<< ( std::ostream&, const DC7unsrt& );

   DC7unsrt( void );
   DC7unsrt( const DC7unsrt& v );
   explicit DC7unsrt( const double v[7] );
   explicit DC7unsrt( const G6& v );
   DC7unsrt(const Cell& c);
   DC7unsrt( const VecN& v );
   explicit DC7unsrt(const D6& ds);
   explicit DC7unsrt(const DeloneTetrahedron& dt);
   DC7unsrt( const std::string& s );
   DC7unsrt( const std::vector<double>& v );
   ~DC7unsrt(void) {}

   DC7unsrt& operator= (const DC7unsrt& v);
   DC7unsrt& operator= (const std::string& s);
   DC7unsrt& operator= (const G6& v);
   DC7unsrt& operator= (const D6& v);
   DC7unsrt& operator= (const DeloneTetrahedron& v);
   DC7unsrt& operator= (const Cell& v);

   static double DistanceBetween( const DC7unsrt& v1, const DC7unsrt& v2 );
private:
   int m_reduced;

};

#endif

