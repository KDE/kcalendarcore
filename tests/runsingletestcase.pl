#!/usr/bin/perl

# This file is part of the kcalcore library.
#
# Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
# Copyright (C) 2005 Reinhold Kainhofer <reinhold@kainhofer.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

# This little script runs a test program on a given (calendar) file and 
# compares the output to a reference file. All discrepancies are shown 
# to the user. Usage:
#      runtestcase.pl appname testfile.ics
# The application/script appname is required to take two arguments:
#      appname inputfile outputfile
# where inputfile is the file to be used as input data, and the output of the
# program will go to outputfile (=testfile.ics.out if called through 
# runtestcase.pl). That outputfile is then compared to the reference file
# testfile.ics.ref.

if ( @ARGV != 3 ) {
  print STDERR "Missing arg! Arguments: testapp identifier filename \n";
  exit 1;
}

$app = quotemeta $ARGV[0];
$id = quotemeta $ARGV[1];
$file = $ARGV[2];            # no quotemeta here, as the regexp does what's
$file =~ /^(.*)\.[^\.]*$/;   # necessary to the filenames

$MAXERRLINES=25;


my $outfile = $file;
$outfile =~ /\/([^\/]*)$/;
$outfile = "$file.$id.out";

if ( $^O eq "MSWin32" || $^O eq "msys" ) {
  $testcmd = "$app $file $outfile 2> nul";
} else {
  $testcmd = "$app $file $outfile 2> /dev/null";
}

#print "CMD $testcmd\n";

if ( system( $testcmd ) != 0 ) {
  print STDERR "Error running $app\n";
  exit 1;
}

checkfile( $file, $outfile );

exit 0;

sub checkfile()
{
  my $file = shift;
  my $outfile = shift;

  $cmd = 'diff -u -w -B -I "^DTSTAMP:[0-9ZT]*" -I "^LAST-MODIFIED:[0-9ZT]*" -I "^CREATED:[0-9ZT]*" -I "^DCREATED:[0-9ZT]*" -I "^PRODID:.*" -I "X-UID=[0-9]*" '."$file.$id.ref $outfile";
  if ( !open( DIFF, "$cmd|" ) ) {
    print STDERR "Unable to run diff command on the files $file.$id.ref and $outfile\n";
    exit 1;
  }

  $errors = 0;
  $errorstr = "";
  while ( <DIFF> ) {
    $line = $_;
    next if ($line =~ m/^[+-]\s*(DTSTAMP|LAST-MODIFIED|CREATED|DCREATED|PRODID|X-UID)/);
    next if ($line =~ m/^[+-]\s*$/);
    next if ($line =~ m/No newline at end of file/);
    next if ($outfile =~ m+/Compat/+ && $line =~ m/^[+-](SEQUENCE|PRIORITY|ORGANIZER:MAILTO):/);
    if ( $line =~ /^[+-][^+-]/ ) {
      # it's an added/deleted/modified line. Register it as an error
      $errors++;
    }
    $errorstr .= $line;
  }

  if ( $errors > 0 ) {
    print "~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n";
    print "Checking '$outfile':\n";
    print $errorstr;
    print "Encountered $errors errors\n";

    if ( !open( ERRLOG, ">>FAILED.log" ) ) {
      print "Unable to open FAILED.log";
    };
    print ERRLOG "\n\n\n~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=\n\n\n";
    print ERRLOG "Checking '$outfile':\n";
    print ERRLOG "Command: $testcmd\n";
    print ERRLOG $errorstr;

    if ( -e "$file.$id.fixme" ) {
      if ( !open( FIXME, "$file.$id.fixme" ) ) {
        print STDERR "Unable to open $file.fixme\n";
        exit 1;
      }
      my $firstline = <FIXME>;
      $firstline =~ /^(\d+) known errors/;
      my $expected = $1;
      if ( $expected == $errors ) {
        print ERRLOG "\n  EXPECTED FAIL: $errors errors found.\n";
        print ERRLOG "    Fixme:\n";
        while( <FIXME> ) {
          print ERRLOG "      ";
          print ERRLOG;
        }
      } else {
        print ERRLOG "\n  UNEXPECTED FAIL: $errors errors found, $expected expected.\n";
        exit 1;
      }
    } else {
      print ERRLOG "\n  FAILED: $errors errors found.\n";
#       system( "touch FAILED" );
      exit 1;
    }
  } else {
     unlink($outfile);
  }
}
