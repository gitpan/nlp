##
##  libCap.pl -- Printer Capability Database functions
##  Copyright (c) Ralf S. Engelschall, All Rights Reserved.
##

package Cap;

%CapAlias = ();
%CapEntry = ();

#   This procedure will load the configuration entries from
#   the cap file into an associative array
#
#   test|schrimsi|...|\
#       :abc=Hallo:...
#
#   cap{'test', 'ALIASES'} = ( 'Schrimsi', ... );
#   cap{'test', 'ALIASES'} = ( 'Schrimsi', ... );
#   cap{'test', 'abc'} = 'Hallo';

sub LoadCap {
    local($cffile) = @_;
    local(@lbuffer, $cline, $oline, $line, @keys, $key,
          @entries, $entry, $arg, $rest );

    @lbuffer = ();
    $cline = "";
    open(CF, "<$cffile") || die "error";
    while(<CF>) {
        chop($_) if (m|\n$|);        # remove end of line marker
        next if (m|^#| || m|^[ \t]*$|);    # ignore comment and blank lines
        if (m|.*\\$|) {
             $_ =~ s|\\$||;          # remove continuation-backstick
             $cline = $cline . $_;
        }
        else {
             $oline = $cline . $_;
             $oline =~ s|\\:|&%&|g;  # protect \: (escaped colon) 
             $oline =~ s|: *:|:|g;   # remove empty fields 
             push(@lbuffer, $oline);
             $cline = "";
        }
    }
    close(CF);
    
    foreach $line (@lbuffer) {
        ($name, $rest) = split(':', $line, 2);
        ($printer, $aliases) = split('\|', $name, 2);
        $CapAlias{$printer} = $aliases;
        @entries = split(':', $rest);
        foreach $entry (@entries) {
            $entry =~ s|&%&|:|g;    # unprotect \: (escaped colon)
            ($key, $arg) = split('=', $entry, 2);
            $CapEntry{$printer, $key} = $arg;
        }
    }
}

1;

##EOF##
