#!/usr/bin/perl -w

use strict;

$trace_path = $1;
print "Processing trace files in $trace_path";

@process_files = <$trace_path/*>; 
