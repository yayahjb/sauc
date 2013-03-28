sauc: \
    BasicDistance.cpp    \
    BasicDistance.h    \
    Cell.cpp \
    Cell.h \
    sauc.cpp \
    NCDist.cpp \
    Reducer.cpp \
    Reducer.h \
    V7.cpp \
    V7.h \
    unitcell.h \
    TNear.h
	g++ -Wall  -O3 -g -o sauc \
    BasicDistance.cpp    \
    Cell.cpp \
    sauc.cpp \
    NCDist.cpp \
    Reducer.cpp \
    V7.cpp
