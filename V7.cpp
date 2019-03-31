
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Cell.h"
#include "G6.h"
#include "Mat66.h"
#include "Vec_N_Tools.h"
#include "V7.h"
#include "vector_3d.h"

#include "Vec_N_Tools.h"
#include "S6M_SellingReduce.h"

V7::V7( void )
: VecN( 7 ) {
   m_dim = 7;
   m_valid = true;
}

V7::V7( const V7& v )
: VecN( 7 ) {
   m_dim = 7;
   m_vec = v.m_vec;
   m_valid = v.m_valid;
}

V7::V7( const double v[7] )
: VecN( 7 ) {
    m_dim = 7;
    m_vec[0] = v[0];
    m_vec[1] = v[1];
    m_vec[2] = v[2];
    m_vec[3] = v[3];
    m_vec[4] = v[4];
    m_vec[5] = v[5];
    m_vec[6] = v[6];
    m_valid = true;
}

V7::V7(const Cell& c)
: VecN( 7 ) {
   const G6& g6= G6(c.Cell2V6());
   const V7 v7(g6);
   m_vec.resize(7);
   m_dim = 7;
   for( size_t i=0; i<7; ++i ) m_vec[i] = v7[i];
   m_valid=true;
}


V7::V7( const VecN& v )
: VecN( v ) {
   if (v.size( ) == 6) {
      G6 g6(v);
      const V7 v7(g6);
      m_vec.resize(7);
      m_dim = 7;
      for( size_t i=0; i<7; ++i ) m_vec[i] = v7[i];
   }
  m_valid = true;
}

V7::V7(const G6& v6)
   : VecN(7) {
   const double& g1 = v6[0];
   const double& g2 = v6[1];
   const double& g3 = v6[2];
   const double& g4 = v6[3];
   const double& g5 = v6[4];
   const double& g6 = v6[5];
   double& aedge = m_vec[0];
   double& bedge = m_vec[1];
   double& cedge = m_vec[2];
   double& asedge= m_vec[3];
   double& bsedge = m_vec[4];
   double& csedge = m_vec[5];
   double& crtvol = m_vec[6];
   const Cell cell( v6 );
   const Cell cI( cell.Inverse() );
   const G6  v6inverse( cI );
  
   Mat66 m;
   G6 reducedDirect;
   G6 reducedBase;
   int reduced=0;
   CS6M_G6Reduce(v6,reducedDirect,reduced);
   CS6M_G6Reduce(v6inverse,reducedBase,reduced);

   aedge = std::sqrt(fabs(reducedDirect[0]));
   bedge = std::sqrt(fabs(reducedDirect[1]));
   cedge = std::sqrt(fabs(reducedDirect[2]));

   asedge = std::sqrt(1.0/reducedBase[0]);
   bsedge = std::sqrt(1.0/reducedBase[1]);
   csedge = std::sqrt(1.0/reducedBase[2]);

   crtvol = pow(  cell.Volume(), 1.0/3.0 );
   m_dim=7;
   m_valid = true;
 
}

V7::V7( const std::string& s ) 
: VecN(7)
{
  m_vec = Vec_N_Tools::FromString( s );
  m_dim = m_vec.size();
  m_valid = true;
  if ( m_dim != 7 ) throw "bad dimension in V7 from a string";
}

V7::V7( const std::vector<double>& v ) 
: VecN( v.size() )
{
  m_dim = v.size();
  if ( m_dim != 7 ) throw "bad dimension in V7 from a vector";
  m_vec = v;
  m_valid = true;
}

double V7::DistanceBetween( const V7& v1, const V7& v2 ) {
   double sum = 0.0;
   for( size_t i=0; i<7; ++i )  sum += (v1[i]-v2[i])*(v1[i]-v2[i]);
   return sqrt( sum );
}

V7& V7::operator= ( const V7& v ) {
   m_vec = v.m_vec;
   m_dim = v.m_dim;
   m_valid = v.m_valid;
   return *this;
}

V7& V7::operator= (const std::string& s)
{
   (*this) = V7(s);
   return *this;
}

V7& V7::operator= (const G6& v6)
{
   (*this) = V7(v6);

   return *this;
}



V7& V7::operator= (const Cell& v)
{
   (*this) = V7(v);
   return *this;
}

std::ostream& operator<< ( std::ostream& o, const V7& v ) {
   std::streamsize oldPrecision = o.precision();
   o << std::fixed <<std::setprecision(3);
   for( size_t i=0; i<v.size( ); ++i )
      o << std::setw(9) << v[i] << " ";
   o << std::setprecision(oldPrecision);
   o.unsetf(std::ios::floatfield);
   return o;
}
