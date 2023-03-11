#!/usr/bin/expect
set path_exec [lindex $argv 0];
set year [lindex $argv 1];
set month [lindex $argv 2];
set day [lindex $argv 3];
set hour [lindex $argv 4];
set ntraj [lindex $argv 5];	#number of trajectories in a file
set lat [lindex $argv 6];
set lon [lindex $argv 7];
set alt [lindex $argv 8];
set length [lindex $argv 9];
set vertical [lindex $argv 10];
set hight [lindex $argv 11];
set nbr_data [lindex $argv 12];
set out_dir [lindex $argv 13];
set out_file [lindex $argv 14];

set timeout -1

spawn $path_exec

expect "           0           0           0           0"
send -- "$year $month $day $hour\n"

expect "           1"
send -- "$ntraj\n"

expect "   40.0000000      -90.0000000       50.0000000    "
send -- "$lat $lon $alt\n"

expect "          48"
send -- "$length\n"

expect "           0"
send -- "$vertical\n"

expect "   10000.0000    "
send -- "$hight\n"

expect "           1"
send -- "$nbr_data\n"

set path0 ""
set file0 ""

for {set i 0} {$i < $nbr_data} {incr i 1} {
	if {$i == 0} {
		set path [lindex $argv [expr {14+2*$i}]];
		set file [lindex $argv [expr {14+2*$i+1}]];
		expect " |main|sub|data|";
		send -- "$path\n";
		expect " file_name";
		send -- "$file\n";
		set path0 $path;
		set file0 $file;
	} else {
		set path [lindex $argv [expr {14+2*$i}]];
		set file [lindex $argv [expr {14+2*$i+1}]];
		expect " $path0";
		send -- "$path\n";
		expect " $file0";
		send -- "$file\n";
		set path0 $path;
		set file0 $file;
	}
}

expect "/main/trajectory/output/"
send -- "$out_dir\n"

expect "file_name"
send -- "$out_file$year$month$day$hour\n"


expect eof
