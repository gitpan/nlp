##
##   libUtil.pl -- utility functions
##   Copyright (c) 1994 Ralf S. Engelschall, <rse@en.muc.de>
##


package Util;

#   define 4.4BSD exit codes
%EX = (
    'OK',     0,
    'USAGE', 64,
    'OSERR', 71,
    'IOERR', 74
);

sub System {
    local($cmd) = $_[0];
	local($rc);

    &Dbg("system(\"$cmd\")");
    $rc = system("$cmd");
	return $rc;
}

sub Dbg {
    local($str) = $_[0];

    if ($Main::option{'debug'} eq 'true') {
        print STDERR "DBG: $str\n";
    }
}

sub Msg {
    local($msg) = $_[0];

    if ($Main::option{'verbose'} eq 'true') {
        print STDERR "$msg\n";
    }
}

sub Warn {
    local($msg) = $_[0];

    print STDERR "WARNING: $msg\n";
}

sub Error {
    local($msg) = $_[0];

    print STDERR "ERROR: $msg\n";
}

1;

#EOF#
