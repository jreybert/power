#!/usr/bin/perl -w

use List::Util qw(sum);
use Getopt::Std;

use strict;

# declare the perl command line flags/options we want to allow
my %options=();

getopts("hw:", \%options);

my $weight_set = 0;
my ($weight_1, $weight_2, $weight_3, $weight_4, $weight_5);
if ( $options{w} ) {
  $weight_set = 1;
  ($weight_1, $weight_2, $weight_3, $weight_4, $weight_5) = split(':', $options{w});
}

print "@ARGV";

if (@ARGV != 1) {
  print "Usage: process_trace.pl trace_path\n";
  exit;
}

my $trace_path = $ARGV[0];


print "Processing trace files in $trace_path\n";

my @process_files = <$trace_path/*.proc>;
my $ipmi_filename = "$trace_path/freq_stat";
my $temp_filename = "$trace_path/temp_stat";

my @trace_array;
my %HoA_time;
my %HoA_ipmi;
my %HoA_temp;

open my $ipmi_file, '<', $ipmi_filename;
while ( <$ipmi_file> ) {
  next if /^\s*#/;             # skip comment lines
  chomp;
  my ($time_value, $ipmi_real_watt, $rest) = split(' ');
  $HoA_ipmi{$time_value} = $ipmi_real_watt;
}
close $ipmi_file;

open my $temp_file, '<', $temp_filename;
while ( <$temp_file> ) {
  next if /^\s*#/;             # skip comment lines
  chomp;
  my ($time_value, @temp_values) = split(' ');
  $HoA_temp{$time_value} = sum(@temp_values);
}
close $temp_file;


foreach my $proc_filename (@process_files) {
#  print "$file\n";
  
  open my $proc_file, '<', $proc_filename;
  while ( my $line =  <$proc_file> ) {

    chomp $line;

    if ( $line =~ /^\s*# weight/) {
      if ( !$weight_set ) {
        (my $null, my $null2, $weight_1, $weight_2, $weight_3, $weight_4, $weight_5) = split(' ', $line);
      }
      next;
    }

    if ( $line =~ /^\s*#/) {
      next;
    }

    my ($time_value, $counter_1, $counter_2, $counter_3, $counter_4, $counter_5, $core_used) = split(' ', $line);

    my $eval_watt = 0;
    $eval_watt += $counter_1 / $weight_1;
    $eval_watt += $counter_2 / $weight_2;
    $eval_watt += $counter_3 / $weight_3;
    $eval_watt += $counter_4 / $weight_4;
    $eval_watt += $counter_5 / $weight_5;


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
  print $data_global "$seconds $tmp_total $HoA_ipmi{$seconds} $HoA_temp{$seconds}\n";
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
