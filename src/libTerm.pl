##
##   libTerm.pl -- terminal I/O functions
##   Copyright (c) Ralf S. Engelschall, <rse@en.muc.de>
##

package Term;

sub YesPrompt {
    local($prompt, $default) = @_;
	local(*TERMIN, *TERMOUT);
	local($y, $n, $rc);

    # open the terminal
    open(TERMOUT, ">/dev/tty");
    open(TERMIN, "</dev/tty");

    # give prompt and read response
    $y = $default eq 'y' ? "Y" : "y";
    $n = $default eq 'n' ? "N" : "n";

	while (1) {
        print TERMOUT $prompt . " ($y/$n): ";
        $response = <TERMIN>;

        # strip off the newline character
        $response =~ s|\n$||;
    
        if (       $response eq 'y' 
    	  	   ||  $response eq 'Y' 
    		   || ($response eq '' && $default eq 'y')) {
    	    $rc = 1;
			last;
    	}
    	elsif (    $response eq 'n' 
    		   ||  $response eq 'N' 
    		   || ($response eq '' && $default eq 'n')) {
    	    $rc = 0;
			last;
    	}
    	else {
			print TERMOUT "**WRONG INPUT, ONLY [yYnN<cr>] ALLOWED!**\n";
    	}
	}

    # close the terminal
	close(TERMOUT);
	close(TERMIN);

	return $rc;
}

1;

#EOF#
