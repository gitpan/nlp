##
##   libFile.pl -- file I/O functions
##   Copyright (c) Ralf S. Engelschall, All Rights Reserved.
##


package File;

sub Truncate {
    local($file) = $_[0];

    open(FP, ">$file");
    close(FP);
}

sub Append {
    local($ofile, $afile) = @_;

    open(FPO, ">>$ofile");
    open(FPI, "<$afile");
    while (<FPI>) {
        print FPO $_;
    }
    close(FPI);
    close(FPO);
}

sub Copy {
    local($ofile, $afile) = @_;

    open(FPO, ">$ofile");
    open(FPI, "<$afile");
    while (<FPI>) {
        print FPO $_;
    }
    close(FPI);
    close(FPO);
}

sub IsASCIIFile {
    local($file) = $_[0];

    open(FP, "<$file");
    $isascii = 1;
    while (($n = read(FP, $buf, 16384)) > 0) {
        if ($buf !~ m|^[\n\t \!\"\#\$\%\&\'\(\)\*\+\,\-\.\/0-9\:\;\<\=\>\?\@A-Z\[\\\]\^\_\`a-z\{\|\}\~]+$|) {
            $isascii = 0;
            last;
        }
    }
    close(FP);
    return ($isascii);
}

sub FileType {
    local($file) = $_[0];
    local($buffer);
    local(*FP);
    local($len);
    local($type);

    open(FP, "<$file");
    $len = read(FP, $buffer, 16384, 0);
    $type = "unknown";
    if (unpack("h4", $buffer) eq '7f20') {
        $type = 'DVI';
    }
    if (unpack("a2", $buffer) eq '%!') {
        $type = 'Postscript';
    }
    if ($type eq 'unknown') {
        seek(FP, 0, 0);
        $isascii = 1;
        while (($n = read(FP, $buffer, 16384)) > 0) {
            if ($buffer !~ m|^[\n\t \!\"\#\$\%\&\'\(\)\*\+\,\-\.\/0-9\:\;\<\=\>\?\@A-Z\[\\\]\^\_\`a-z\{\|\}\~]+$|) {
                $isascii = 0;
                last;
            }
        }
        if ($isascii) {
            $type = 'ASCII';
        }
        else {
            $type = 'Binary';
        }
    }
    close(FP);
    return ($type);
}

sub StripPrintModeCmds {
    local ($from, $to) = @_;
    local (*IN, *OUT);

    open(IN,  "<$from");
    open(OUT, ">$to");
    while (<IN>) {
        next if (m|^\033%-12345X.*|);
        next if (m|^\@PJL.*|);
        print OUT $_;
    }
    close(OUT);
    close(IN);
}

$uniq = 1;
@tmpfiles = ();

sub Tmpfile {
    local($file);
    local($name);

    $name = getpwuid($<);
    $file = "/tmp/tmp.$name.${Main::prgname}." . time() . ".$uniq";
    $uniq++;
    sleep(1);
    return ($file);
}

sub TmpfileCleanup {
    foreach $file (@tmpfiles) {
        if (-f $file) {
            unlink($file);
        }
    }
}


#EOF#
