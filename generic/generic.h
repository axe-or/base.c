#pragma once

#define M_GLUE0(X, Y)  X##Y
#define M_GLUE1(X, Y)  M_GLUE0(X, Y)
#define M_GLUE2(X, Y)  M_GLUE1(X, Y)
#define M_GLUE3(X, Y)  M_GLUE2(X, Y)
#define M_GLUE4(X, Y)  M_GLUE3(X, Y)
#define M_GLUE5(X, Y)  M_GLUE4(X, Y)
#define M_GLUE6(X, Y)  M_GLUE5(X, Y)
#define M_GLUE7(X, Y)  M_GLUE6(X, Y)
#define M_GLUE8(X, Y)  M_GLUE7(X, Y)
#define M_GLUE9(X, Y)  M_GLUE8(X, Y)
#define M_GLUE10(X, Y) M_GLUE9(X, Y)
#define M_GLUE11(X, Y) M_GLUE10(X, Y)
#define M_GLUE12(X, Y) M_GLUE11(X, Y)
#define M_GLUE13(X, Y) M_GLUE12(X, Y)
#define M_GLUE14(X, Y) M_GLUE13(X, Y)
#define M_GLUE(X, Y)   M_GLUE14(X, Y)

#define M_EXP0(X)  X
#define M_EXP1(X)  M_EXP0(X)
#define M_EXP2(X)  M_EXP1(X)
#define M_EXP3(X)  M_EXP2(X)
#define M_EXP4(X)  M_EXP3(X)
#define M_EXP5(X)  M_EXP4(X)
#define M_EXP6(X)  M_EXP5(X)
#define M_EXP7(X)  M_EXP6(X)
#define M_EXP8(X)  M_EXP7(X)
#define M_EXP9(X)  M_EXP8(X)
#define M_EXP10(X) M_EXP9(X)
#define M_EXP11(X) M_EXP10(X)
#define M_EXP12(X) M_EXP11(X)
#define M_EXP13(X) M_EXP12(X)
#define M_EXP14(X) M_EXP13(X)
#define M_EXP(X)   M_EXP14(X)


