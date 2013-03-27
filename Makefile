sauc: \
    BasicDistance.cpp    \
    BasicDistance.h    \
    Cell.cpp \
    Cell.h \
    sauc.cpp \
    CNCDist.c \
    Reducer.cpp \
    Reducer.h \
    V7.cpp \
    V7.h \
    unitcell.h \
    TNear.h \
    vector_3d.cpp
	g++ -Wall  -O3 -g -o sauc \
    BasicDistance.cpp    \
    Cell.cpp \
    sauc.cpp \
    CNCDist.c \
    Reducer.cpp \
    V7.cpp
