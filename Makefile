sauc: \
    BasicDistance.cpp    \
    BasicDistance.h    \
    Cell.cpp \
    Cell.h \
    sauc.cpp \
    NCDist.h \
    Reducer.cpp \
    Reducer.h \
    V7.cpp \
    V7.h \
    unitcell.h \
    rhrand.h \
    triple.h \
    TNear.h
	g++ -Wall  -O3 -Wall -DUSE_LOCAL_HEADERS -g -o sauc \
    BasicDistance.cpp    \
    Cell.cpp \
    sauc.cpp \
    Reducer.cpp \
    V7.cpp
