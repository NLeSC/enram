#! /bin/bash
#
# Makefile
#gcc -Wall -O MF_binary_to_ODIM_vol_h5.c -o MF_binary_to_ODIM_vol_h5 -I$HOME/include -L$HOME/lib -lhdf5_hl -lhdf5 -lz -lm

#Setting names of program

SOURCE="MF_binary_to_ODIM_vol_h5.c"
BINARY="$HOME/bin/MF_binary_to_ODIM_vol_h5"

#Setting compiler and options

COMPILE="gcc -Wall -O"

#Setting include and library directories

IPATH="-I$HOME/include"
LPATH="-L$HOME/lib"

#Setting libraries

LIBRARIES="-lhdf5_hl -lhdf5 -lz -lm"

#Compiling of source

$COMPILE $SOURCE -o $BINARY $IPATH $LPATH $LIBRARIES
