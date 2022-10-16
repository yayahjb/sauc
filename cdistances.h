#ifdef __cplusplus
extern "C" {
#endif


#ifndef CDISTANCES_H
#define CDISTANCES_H

/* From NCDist.h */

double NCDist(double gvec1[6], double gvec2[6]);
double DC7Dist(double gvec1[6], double  gvec2[6]);
double DC7sqDist(double gvec1[6], double  gvec2[6]);
double DC7Distraw(double gvec1[6], double  gvec2[6]);
double DC7sqDistraw(double gvec1[6], double  gvec2[6]);
double DC7unsortedsqDist(double v1[7],double v2[7]);
double DC7unsrtDist(double gvec1[6], double  gvec2[6]);
double DC7unsrtsqDist(double gvec1[6], double  gvec2[6]);
double DC7unsrtDistraw(double gvec1[6], double  gvec2[6]);
double DC7unsrtsqDistraw(double gvec1[6], double  gvec2[6]);
double DC10Dist(double gvec1[6], double  gvec2[6]);
double DC10sqDist(double gvec1[6], double  gvec2[6]);
double DC10Distraw(double gvec1[6], double  gvec2[6]);
double DC10sqDistraw(double gvec1[6], double  gvec2[6]);

/* From CS6Dist.h */

double CS6Dist(double gvec1[6], double gvec2[6]);

/* From D7Dist.h */

double D7Dist(double gvec1[7], double gvec2[7]);

#endif

#ifdef __cplusplus
}
#endif

