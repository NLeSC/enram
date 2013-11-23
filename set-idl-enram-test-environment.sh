#!/bin/bash

# execute this script in a terminal by:
# . set-idl-enram-test-environment.sh
# (don't omit the dot at the beginning)
# then stat idl or idlde from that same terminal with
# idl &
# or
# idlde &

# set IDL paths
. /home/daisycutter/exelis/idl/bin/idl_setup.bash

# define where the IDL binary is located:
export IDL_DIR=~/exelis/idl82;

# define in which directories IDL will search for functions, scripts, and so on:
export IDL_PATH=\+$IDL_DIR/lib:+$IDL_DIR/external/lib:+/home/daisycutter/enram/src;

# define the location of the startup file:
export IDL_STARTUP='/home/daisycutter/enram/src/idl_startup.pro'

# define where the different ENRAM directories are:
export IDL_ENRAM_VOL2BIRDLIB='/home/daisycutter/enram/src/vol2bird/vol2bird.so'
export IDL_ENRAM_SRC_IO='/home/daisycutter/enram/src/io/'
export IDL_ENRAM_RAW_DATA='/home/daisycutter/enram/testdata/raw/'
export IDL_ENRAM_ODIM_DATA='/home/daisycutter/enram/testdata/harmonized/odim/'
export IDL_ENRAM_BIRD_DATA='/home/daisycutter/enram/testdata/harmonized/bird/'
export IDL_ENRAM_CLUTTERMAPS='/home/daisycutter/enram/testdata/harmonized/cluttermaps/'
export IDL_ENRAM_STATISTICS='/home/daisycutter/enram/testdata/harmonized/statistics/'
export IDL_ENRAM_CLUTTER_SENSITIVTY='/home/daisycutter/enram/testdata/harmonized/clutter-sensitivity/'
export IDL_ENRAM_VISUALIZATION_OUTPUT='/home/daisycutter/enram/visualization-output/'
