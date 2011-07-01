#!/usr/bin/perl -w

use List::Util qw(sum);

use strict;

if (@ARGV != 1) {
  print "Usage: process_trace.pl trace_path\n";
  exit;
}

my $trace_path = $ARGV[0];


print "Processing trace files in $trace_path\n";

my @process_files = <$trace_path/*.proc>;
my $ipmi_filename = "$trace_path/ipmi_freq";

my @trace_array;
my %HoA_time;
my %HoA_ipmi;

open my $ipmi_file, '<', $ipmi_filename;
while ( <$ipmi_file> ) {
  next if /^\s*#/;             # skip comment lines
  chomp;
  my ($time_value, $ipmi_real_watt, $rest) = split(' ');
  $HoA_ipmi{$time_value} = $ipmi_real_watt;
}
close $ipmi_file;

foreach my $proc_filename (@process_files) {
#  print "$file\n";
  
  open my $proc_file, '<', $proc_filename;
  while ( <$proc_file> ) {
    next if /^\s*#/;             # skip comment lines
    chomp;
    my ($time_value, $unh_core_cycle, $inst_retired, $L1_misses, $L2_misses, $LLC_misses, $core_used) = split(' ');
    my $eval_watt = $unh_core_cycle / 1_000_000_000;

    $inst_retired /= 1_000_000_000;

    $eval_watt += $inst_retired + $inst_retired / 10;

#    if ( 0 < $inst_retired and $inst_retired <= 0.5 )    { $eval_watt += 1; }
#    elsif ( 0.5 < $inst_retired and $inst_retired <= 1 ) { $eval_watt += 2; }
#    elsif ( 1 < $inst_retired and $inst_retired <= 2 )   { $eval_watt += 3; }
#    elsif ( 2 < $inst_retired and $inst_retired <= 4 )   { $eval_watt += 5; }
#    else { $eval_watt += 6; }

    $eval_watt += $L1_misses / 1_000_000_000;
    $eval_watt += $L2_misses / 1_000_000;
    #$eval_watt += $LLC_misses / 1_000;

    push @{ $HoA_time{$time_value} }, $eval_watt;
  }
  close $proc_file;
  #push @trace_array, [@lines];
}

my @array_power_real;
my @array_power_estim;
my $last_second;

open my $data_global, '>', "data_global.gnuplot";
open my $data_per_core, '>', "data_per_core.gnuplot";

print $data_per_core "# time core_1 core_2 ...\n";
print $data_global "# time estimated real\n";

# print the whole thing
foreach my $seconds (  sort {$a <=> $b} keys %HoA_time ) {
  $last_second = $seconds;
  print $data_per_core "$seconds @{ $HoA_time{$seconds} }\n";
  my $tmp_total = 480;
  ($tmp_total+=$_) for  @{ $HoA_time{$seconds} };
  print $data_global "$seconds $tmp_total $HoA_ipmi{$seconds}\n";
  push @array_power_real, $HoA_ipmi{$seconds};
  push @array_power_estim, $tmp_total;
}

close $data_global;
close $data_per_core;

my $total_real = 0;
($total_real+=$_) for @array_power_real;
my $total_est = 0;
($total_est+=$_) for @array_power_estim;

print "Total consumption (joules):
       Real:      $total_real
       Estimated: $total_est\n";

#my $aref = $trace_array[0];
#print " $trace_array[0][0] $trace_array[0][26] \n";
#print $#trace_array;
#print $#{$aref};
#print @${aref};
#print "\n";
