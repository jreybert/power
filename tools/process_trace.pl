#!/usr/bin/perl -w

use POSIX qw(strftime);

use List::Util qw(sum);
use File::Copy;
use Getopt::Std;

use strict;

# declare the perl command line flags/options we want to allow
my %options=();

getopts("hw:o:i:d:", \%options);

my $weight_set = 0;
my ($weight_1, $weight_2, $weight_3, $weight_4, $weight_5);
if ( $options{w} ) {
  $weight_set = 1;
  ($weight_1, $weight_2, $weight_3, $weight_4, $weight_5) = split(':', $options{w});
}


my $now_string = POSIX::strftime("%Y_%m_%d_%H_%M_%S", localtime);
my $out_directory_path = "$now_string";
if ( $options{d} ) {
  my $tmp_name = $options{d};
  if ( -d "$tmp_name" ) {
    my $directory_inc = 0;
    while ( -d "${tmp_name}_${directory_inc}" ) {
      $directory_inc++;
    }
    $out_directory_path = "${tmp_name}_${directory_inc}";
  }
  else {
    $out_directory_path = "$tmp_name";
  }
}
mkdir "$out_directory_path";


my $out_path_prefix = "${out_directory_path}/";
if ( $options{o} ) {
  $out_path_prefix = "${out_directory_path}/$options{o}";
}

#print "@ARGV";

my $trace_path = "/tmp/trace";
if ( $options{i} ) {
  $trace_path = "$options{i}";
}


#if (@ARGV != 1) {
#  print "Usage: process_trace.pl trace_path\n";
#  exit;
#}
#
#$trace_path = $ARGV[0];

# copy trace files just in case
mkdir "$out_directory_path/trace";
my @trace_files = glob("$trace_path/*");
foreach (@trace_files) {
  copy("$_","$out_directory_path/trace/.");
}

print "Processing trace files in $trace_path\n";

my @process_files = <$trace_path/*.proc>;
my $ipmi_proc_filename = "$trace_path/freq_stat";
#my $temp_filename = "$trace_path/temp_stat";
my $hw_filename = "$trace_path/hw_stat";

my @trace_array;
my %HoA_estimated;
my %HoA_ipmi_proc;

my %HoA_ipmi_global;
my %HoA_temp_global;

open my $ipmi_proc_file, '<', $ipmi_proc_filename;
while ( <$ipmi_proc_file> ) {
  next if /^\s*#/;             # skip comment lines
  chomp;
  my ($time_value, $ipmi_real_watt, $rest) = split(' ');
  $HoA_ipmi_proc{$time_value} = $ipmi_real_watt;
}
close $ipmi_proc_file;
#
#open my $temp_file, '<', $temp_filename;
#while ( <$temp_file> ) {
#  next if /^\s*#/;             # skip comment lines
#  chomp;
#  my ($time_value, @temp_values) = split(' ');
#  $HoA_temp{$time_value} = sum(@temp_values);
#}
#close $temp_file;

open my $hw_file, '<', $hw_filename;
while ( <$hw_file> ) {
  next if /^\s*#/;             # skip comment lines
  chomp;
  my ($time_value, $ipmi_watt, @temp_values) = split(' ');
#  $HoA_temp_global{$time_value} = sum(@temp_values);
  push @{ $HoA_temp_global{$time_value} }, @temp_values;
  $HoA_ipmi_global{$time_value} = $ipmi_watt;
}
close $hw_file;


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


    push @{ $HoA_estimated{$time_value} }, $eval_watt;
  }
  close $proc_file;
  #push @trace_array, [@lines];
}

my @array_power_real;
my @array_power_estim;

open my $data_global_est, '>', $out_path_prefix."data_global_est.gnuplot";
open my $data_global_hw, '>', $out_path_prefix."data_global_hw.gnuplot";
open my $data_per_core_est, '>', $out_path_prefix."data_per_core_est.gnuplot";
open my $data_per_core_hw, '>', $out_path_prefix."data_per_core_hw.gnuplot";

print $data_per_core_est "# time core_1 core_2 ...\n";
print $data_per_core_hw "# time temp1 temp2 ...\n";
print $data_global_est "# time estimated ipmi\n";
print $data_global_hw "# time ipmi sum_temp ...\n";

# print the whole thing
foreach my $seconds (  sort {$a <=> $b} keys %HoA_estimated ) {
  print $data_per_core_est "$seconds @{ $HoA_estimated{$seconds} }\n";
  my $tmp_total = 480;
  ($tmp_total+=$_) for  @{ $HoA_estimated{$seconds} };
  print $data_global_est "$seconds $tmp_total $HoA_ipmi_proc{$seconds}\n";
  push @array_power_real, $HoA_ipmi_proc{$seconds};
  push @array_power_estim, $tmp_total;
}


foreach my $seconds (  sort {$a <=> $b} keys %HoA_ipmi_global ) {
  
  print $data_per_core_hw "$seconds @{ $HoA_temp_global{$seconds} }\n";
  
  my $sum_temp = sum( @{ $HoA_temp_global{$seconds} } );
  print $data_global_hw "$seconds $HoA_ipmi_global{$seconds} $sum_temp\n";

}

close $data_global_est;
close $data_global_hw;
close $data_per_core_est;
close $data_per_core_hw;

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

# POSTSCRIPT
open (my $GNUPLOT, "|gnuplot -p");
print $GNUPLOT <<EOPLOT;

set xlabel "time (s)"
set ylabel "power (W)"
set key left outside

plot  "${out_path_prefix}data_global_est.gnuplot" u 1:2  w lp  t "Estimated" , \\
      "${out_path_prefix}data_global_hw.gnuplot" u 1:2  w lp  t "Real", \\
      "${out_path_prefix}data_global_hw.gnuplot" u 1:(\$3*0.739-578)  w lp  t "Temp"

set term png
set output "${out_path_prefix}curves.png"
replot
EOPLOT

close($GNUPLOT);
