##
##  modInfo.pl -- Printer Information Support
##  Copyright (c) Ralf S. Engelschall, All Rights Reserved.
##

package Info;

sub info {
	local(*option) = @_;

    $printer   = $option{'printer'};
    $found = 0;
	foreach $p (keys(%Cap::CapAlias)) {
		$found = 1 if ($p eq $printer);
	}
	if ($found) {
        $aliases   = $Cap::CapAlias{$printer};
    	$aliases   =~ s/\|/, /g;
    	$type      = $Cap::CapEntry{$printer, 'type'};
    	$mode      = $Cap::CapEntry{$printer, 'mode'};
    	$dpi       = $Cap::CapEntry{$printer, 'dpi'};
    	$duplex    = $Cap::CapEntry{$printer, 'duplex'};
    	$location  = $Cap::CapEntry{$printer, 'location'};
		print "\n";
    	print " Name         : $printer\n";
    	print " Aliases      : $aliases\n";
    	print " Model Type   : $type\n";
    	print " Print Mode   : $mode\n";
    	print " DPI          : $dpi\n";
    	print " Duplex       : $duplex\n";
    	print " Room         : $location\n";
		print "\n";
	}
	else {
		&Util::Warn("Sorry, printer '$printer' not found in printer.cap");
	}
}

sub infoall {
	local(*option) = @_;

    print "\n";
	printf(" Printer Model Type               Print Mode      DPI Duplex Room  \n");
	printf(" ------- ------------------------ -------------- ---- ------ ----- \n");
    foreach $printer (sort(keys(%Cap::CapAlias))) {
        $aliases    = $Cap::CapAlias{$printer};
		$aliases    =~ s/\|/, /g;
		$type       = $Cap::CapEntry{$printer, 'type'};
		$mode       = $Cap::CapEntry{$printer, 'mode'};
		$dpi        = $Cap::CapEntry{$printer, 'dpi'};
		$duplex     = $Cap::CapEntry{$printer, 'duplex'};
		$location   = $Cap::CapEntry{$printer, 'location'};
		$location   =~ s|^([^,]+)[ \t]*,.*|$1|;
		printf(" %-7s %-24s %-14s %4s %-6s %-5s\n",
		       $printer, $type, $mode, $dpi, $duplex, $location );
	}
    print "\n";
}

1;

##EOF##
