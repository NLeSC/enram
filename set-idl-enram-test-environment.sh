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
enramroot='/home/daisycutter/enram/'
export IDL_ENRAM_VOL2BIRDLIB=$enramroot'src/vol2bird/vol2bird.so'
export IDL_ENRAM_SRC_IO=$enramroot'src/io/'
export IDL_ENRAM_DATA=$enramroot'testdata/'
export IDL_ENRAM_RAW_DATA=$enramroot'testdata/raw/'
export IDL_ENRAM_ODIM_DATA=$enramroot'testdata/harmonized/odim/'
export IDL_ENRAM_BIRD_DATA=$enramroot'testdata/harmonized/bird/'
export IDL_ENRAM_CLUTTERMAPS=$enramroot'testdata/harmonized/cluttermaps/'
export IDL_ENRAM_STATISTICS=$enramroot'testdata/harmonized/statistics/'
export IDL_ENRAM_CLUTTER_SENSITIVITY=$enramroot'testdata/harmonized/clutter-sensitivity/'
export IDL_ENRAM_VISUALIZATION_OUTPUT=$enramroot'visualization-output/'
