#!/bin/bash
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

#This should give no warnings or errors
make

./gameoflife < ./tests/glider.txt > ./tests/output.txt
if [ $? -eq 0 ]
then
  echo Test passed: Correct error code
else
  echo Test failed: Wrong error code
fi
diff --ignore-blank-lines ./tests/output.txt ./tests/glider_output.txt
if [ $? -eq 0 ]
then
  echo Test passed: Correct output
else
  echo Test failed: Wrong output
fi
