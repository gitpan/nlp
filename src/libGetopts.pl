##
##   libGetopts.pl -- getopts() function
##   Copyright (c) Ralf S. Engelschall, <rse@en.muc.de>
##

package Getopts;

#   this subroutine is derived from getopts.pl with the enhancement
#   of the "+" metacharater at the format string to allow a list to
#   be build by subsequent occurance of the same option.

sub Getopts {
    local($argumentative, @ARGV) = @_;
    local(@args,$_,$first,$rest);
    local($errs) = 0;
    local($[) = 0;

    @args = split( / */, $argumentative );
    while(@ARGV && ($_ = $ARGV[0]) =~ /^-(.)(.*)/) {
        ($first,$rest) = ($1,$2);
        if ($_ =~ m|^--$|) {
            shift(@ARGV);
            last;
        }
        $pos = index($argumentative,$first);
        if($pos >= $[) {
            if($args[$pos+1] eq ':') {
                shift(@ARGV);
                if($rest eq '') {
                    unless (@ARGV) {
                        print STDERR "Incomplete option: $first (needs an argument)\n";
                        ++$errs;
                    }
                    $rest = shift(@ARGV);
                }
                eval "\$opt_$first = \$rest;";
            }
            elsif ($args[$pos+1] eq '+') {
                shift(@ARGV);
                if($rest eq '') {
                    unless (@ARGV) {
                        print STDERR "Incomplete option: $first (needs an argument)\n";
                        ++$errs;
                    }
                    $rest = shift(@ARGV);
                }
                eval "push(\@opt_$first, \$rest);";
            }
            else {
                eval "\$opt_$first = 1";
                if($rest eq '') {
                    shift(@ARGV);
                }
                else {
                    $ARGV[0] = "-$rest";
                }
            }
        }
        else {
            print STDERR "Unknown option: $first\n";
            ++$errs;
            if($rest ne '') {
                $ARGV[0] = "-$rest";
            }
            else {
                shift(@ARGV);
            }
        }
    }
    return ($errs == 0, @ARGV);
}


1;

#EOF#
