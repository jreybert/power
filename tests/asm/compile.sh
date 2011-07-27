#!/bin/bash

# last param should be .s to compile
output=${!#%.*}

echo gcc $* -o $output
gcc $* -o $output
