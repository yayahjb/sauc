#ifndef BASICDISTANCE_H
#define BASICDISTANCE_H

#include "G6.h"

class BasicDistance

/*
Functions for determining partial distances in G6. They are placed
here in a support class because they are used both in Reducer
and in MakeGaol.
*/
{
public:
   static double g456dist( const G6& v1, const G6& v2 );
   static double g123dist( const G6& v1, const G6& v2 );
};

#endif // BASICDISTANCE_H
