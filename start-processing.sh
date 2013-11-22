#!/bin/bash

# start this script from the terminal by (make sure to enter it exactly):
# . ./start-processing.sh

echo "Do you wish to process the test data or the full data? (Enter 1 or 2)"
select answer in "Test data only" "Full data"; do
    case $answer in
        "Test data only" ) echo "Test data only"; . set-idl-enram-test-environment.sh; break;;
        "Full data" ) echo "Full data"; . set-idl-enram-environment.sh; break;;
    esac
done



idl -e ".r histogram.pro"

idl -e ".r cluttermap.pro"

idl -e ".r clutter_sensitivity.pro"

idl -e ".r flysafe2.pro"


