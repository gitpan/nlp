##
##  modPrint.pl -- the real heart of NLP ;-)
##  Copyright (c) Ralf S. Engelschall, All Rights Reserved.
##

package Print;

sub print {
    local(*option, *files) = @_;
    local($tmpfile, *TMPFP);

    %optionORIG = %option; # save the options, because they
                           # will be transformed while processing a single file
    foreach $file (@files) {
        %option = %optionORIG; # take a fresh new copy of the options
        if ($file eq '-') {
            #   copy stdin to temporary file
            &Util::Dbg("preprocess: STDIN -> tmpfile");
            $tmpfile = &File::Tmpfile();
            open(TMPFP, ">$tmpfile");
            while (<STDIN>) {
                print TMPFP $_;
            }
            close(TMPFP);
            if ($option{'selectpage'} =~ m|^S$|) {
                printf STDERR "Printing ODD pages.\n";
                $option{'selectpage'} = 'o';
                &printFile(*option, $tmpfile);
                printf STDERR "Please turn the paper stack now.\n";
                printf STDERR "We will then print the EVEN pages.\n";
                if (&Term::YesPrompt("Continue", "y") == 0) {
                    print STDERR "**STOPPED**\n";
                    exit(1);
                }
                $option{'selectpage'} = 'e';
                printf STDERR "Printing EVEN pages.\n";
            }
            &printFile(*option, $tmpfile);
            unlink($tmpfile) if (-f $tmpfile);
        }
        elsif (-r $file and -f $file) {
            if ($option{'selectpage'} =~ m|^S$|) {
                printf STDERR "Printing ODD pages.\n";
                $option{'selectpage'} = 'o';
                &printFile(*option, $file);
                printf STDERR "Please turn the paper stack now.\n";
                printf STDERR "We will then print the EVEN pages.\n";
                if (&Term::YesPrompt("Continue", "y") == 0) {
                    print STDERR "**STOPPED**\n";
                    exit(1);
                }
                $option{'selectpage'} = 'e';
                printf STDERR "Printing EVEN pages.\n";
            }
            &printFile(*option, $file);
        }
        else {
            &Util::Error("Cannot open '$file'.\n");
        }
    }
}

sub printFile {
    local(*option, $file) = @_;
    local($tmpfile, $tmpfile2);

    #   strip printer mode commands
    if ($option{'removeheader'} eq 'true') {
        $tmpfile = &File::Tmpfile();
        &File::StripPrintModeCmds($file, $tmpfile);
        $file = $tmpfile;
    }

    #   apply external filter
    if ($option{'filter'} ne 'unset') {
        $tmpfile2 = &File::Tmpfile();
        &Util::System("$option{'filter'} <$file >$tmpfile2");
        $file = $tmpfile2;
    }

    #   determine filetype
    $filetype = &File::FileType($file);
    if ($option{'verbose'} eq 'true') {
        printf("$file ($filetype)\n");
    }

    if ($filetype eq 'ASCII') {
        @output = printASCII(*option, $file);
    }
    elsif ($filetype eq 'DVI') {
        @output = printDVI(*option, $file);
    }
    elsif ($filetype eq 'PDF') {
        @output = printPDF(*option, $file);
    }
    elsif ($filetype eq 'Postscript') {
        @output = printPostscript(*option, $file, 'original');
    }
    else {
        if ($option{'expert'} eq 'true') {
            printf STDERR "WARNING: file '$file' is of type '$filetype'!\n";
            printf STDERR "Threated as ISO-Latin-1.\n";
            @output = printASCII(*option, $file);
        }
        else {
            printf STDERR "WARNING: file '$file' is of type '$filetype'!\n";
            printf STDERR "This file will be threated as ISO-Latin-1 extended ASCII file.\n";
            if (&Term::YesPrompt("Are you sure?", "N") == 1) {
                @output = printASCII(*option, $file);
            }
            else {
                print STDERR "**SKIPPED**\n";
            }
        }
    }

    unlink($tmpfile) if (-f $tmpfile);
    unlink($tmpfile2) if (-f $tmpfile2);
}

sub printASCII {
    local(*option, $file) = @_;

    &Util::Dbg("filter: ASCII");

    #   Default options to make a2ps friendly
    $a2psopt = '-nd -nL -q -b -nc -nu -v -8';

    #   ASCII options
    $a2psopt .= ($option{'fontsize'} eq 'unset' ? "" : " -F$option{'fontsize'}");
    $a2psopt .= ($option{'boldface'} eq 'true' ? " -B" : " -nB");
    $a2psopt .= ($option{'borders'} eq 'true' ? " -s" : " -ns");
    $option{'borders'} = 'false';
    $a2psopt .= " -t$option{'tabindent'}";
    $a2psopt .= ($option{'wraplines'} eq 'true' ? " -f" : " -nf");
    $a2psopt .= ($option{'maxlines'} eq 'unset' ? "" : " -l$option{'maxlines'}");
    $a2psopt .= ($option{'linenumbers'} eq 'true' ? " -n" : " -nn");
    $a2psopt .= ($option{'pageheader'} eq 'unset' ? " -nH" : " '-H$option{'pageheader'}'");
    $a2psopt .= ($option{'metachars'} eq 'true' ? " -ni" : " -i");

    #   Layout Options
    $a2psopt .= ($option{'landscapemode'} eq 'true' ? " -l" : " -p");
    $option{'landscapemode'} = 'false';
    $a2psopt .= ($option{'2page'} eq 'true' ? " -2" : " -1");
    $option{'2page'} = 'false';
    $a2psopt .= ($option{'4page'} eq 'true' ? " -1" : "");

    $tmpfile = &File::Tmpfile();
    &Util::System("$Config::PRG_A2PS $a2psopt <$file >$tmpfile 2>/dev/null");

    &printPostscript(*option, $tmpfile, 'temporary');

    unlink($tmpfile) if (-f $tmpfile);
}

sub printDVI {
    local(*option, $file) = @_;
	local($tmpfile, $tmpfile2, $dvipsopt, $start);

    &Util::Dbg("filter: DVI");

    $dvipsopt = '';
    $dvipsopt .= " -D$option{'dpi'}";

    $tmpfile = &File::Tmpfile();
    &Util::System("$Config::PRG_DVIPS $dvipsopt -o$tmpfile $file 2>/dev/null");
    $tmpfile2 = &File::Tmpfile();
    open(FPI, "<$tmpfile");
    open(FPO, ">$tmpfile2");
    $start = 0;
    while (<FPI>) {
        $start = 1 if (m|^%!PS-Adobe-2.0.*|);
        next if $start == 0;
        print FPO $_;
    }
    close(FPO);
    close(FPI);
    unlink($tmpfile);

    &printPostscript(*option, $tmpfile2, 'temporary');

    unlink($tmpfile2) if (-f $tmpfile2);
}

sub printPDF {
    local(*option, $file) = @_;
	local($tmpfile, $pdftopsopt);

    &Util::Dbg("filter: PDF");

    $pdftopsopt = "";

    $tmpfile = &File::Tmpfile();
    &Util::System("$Config::PRG_PDFTOPS $pdftopsopt $file $tmpfile 2>/dev/null");

    &printPostscript(*option, $tmpfile, 'temporary');

    unlink($tmpfile) if (-f $tmpfile);
}

sub printPostscript {
    local(*option, $file, $filetype) = @_;
    local($outfile);
    local($header);

    &Util::Dbg("filter: Postscript");

    #   prepare tempfile
    $outfile = &File::Tmpfile();
    unlink($outfile);
    &File::Truncate($outfile);

    #   get Postscript magic cookie header line
    open(FP, "<$file");
    $header = <FP>;
    close(FP);

    #   check if source is really of type Postscript
    #   (just be safety!)
    if ($header !~ m|^%!.*|) {
        &Util::Error("Output is not of type Postscript!");
        return;
    }

    #   handle forced Duplex mode
    if ($option{'duplexmode'} eq 'true') {
        if (   $option{'outputfile'} ne 'unset'
            || $Cap::CapEntry{$option{'printer'}, 'duplex'} eq 'yes') {
            &File::Append($outfile, "${Config::libdir}/prologue/ps.duplex.on");
        }
        else {
            &Util::Warn("Printer '" . $option{'printer'} . "' doesn't hava a Duplex unit - ignoring option.");
        }
    }

    #   handle forced Simplex mode
    if ($option{'simplexmode'} eq 'true') {
        if (   $option{'outputfile'} ne 'unset'
            || $Cap::CapEntry{$option{'printer'}, 'duplex'} eq 'yes') {
            &File::Append($outfile, "${Config::libdir}/prologue/ps.duplex.off");
        }
        else {
            &Util::Warn("Printer '" . $option{'printer'} . "' doesn't have a Duplex unit - ignoring option.");
        }
    }

    #   handle Papertray selection
    if ($option{'papertray'} ne 'unset') {
        if ($option{'papertray'} == 0) {
            &File::Append($outfile, "${Config::libdir}/prologue/ps.papertray.upper");
        }
        elsif ($option{'papertray'} == 1) {
            &File::Append($outfile, "${Config::libdir}/prologue/ps.papertray.lower");
        }
        else {
            &Util::Warn("Papertray '" . $option{'papertray'} . "' not known -- ignoring.");
        }
    }

    #   handle Manualfeed mode
    if ($option{'manualfeed'} eq 'true') {
        &File::Append($outfile, "${Config::libdir}/prologue/ps.manualfeed.on");
    }
    else {
        #&File::Append($outfile, "${Config::libdir}/prologue/ps.manualfeed.off");
    }

    #   if anything of the above options were used,
    #   prepend their contents to the top of the output
    if (-s $outfile) {
        $outfile2 = &File::Tmpfile();
        unlink($outfile2);
        open(FP, ">>$outfile2");
        print FP $header;
        close(FP);
        &File::Append($outfile2, $outfile);
        &File::Append($outfile2, $file);
        unlink($file) if ($filetype eq 'temporary');
        $file = $outfile2;
        $filetype='temporary';
    }
    unlink($outfile);

    #   handle page selection (#,-#,#-,#-#,r,o,e,B)
    if ($option{'selectpage'} ne 'unset') {
        $select='';
        $book = 0;
        if ($option{'selectpage'} =~ m|^\d*-*\d*$|) {
            $select = $option{'selectpage'};
        }
        elsif ($option{'selectpage'} =~ m|^[oer]$|) {
            $select = "-$option{'selectpage'}";
        }
        elsif ($option{'selectpage'} =~ m|^B$|) {
            $book = 1;
        }
        else {
            &Util::Warn("Page specification '" . $option{'selectpage'} . "' not known -- ignoring.");
        }
        if ($select ne '') {
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSSELECT -q $select $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
        }
        if ($book == 1) {
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSBOOK -q -s4 $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
        }
    }


    #   handle Borders around Postscript pages
    #   (uses the borders feature of pstops)
    $border = $option{'borders'} eq 'true' ? '-d' : '';

    #   pure portrait with border
    if (   $option{'2page'}         eq 'false'
        && $option{'4page'}         eq 'false'
        && $option{'landscapemode'} eq 'false'
        && $option{'borders'}       eq 'true') {
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSTOPS -q $border '1:0@.9(0.2cm,0.2cm)' $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
    }

    #   pure landscape
    if (   $option{'2page'}         eq 'false'
        && $option{'4page'}         eq 'false'
        && $option{'landscapemode'} eq 'true') {
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSTOPS -q $border '1:0L@.7(21cm,0cm)' $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
    }

    #   only 2 onto 1
    if (   $option{'2page'}         eq 'true'
        && $option{'landscapemode'} eq 'false') {
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSTOPS -q $border '2:0@.49(0cm,14.85cm)+1@.49(10.5cm,14.85cm)' $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
    }

    #   real landscape
    if (   $option{'2page'}         eq 'true'
        && $option{'landscapemode'} eq 'true') {
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSTOPS -q $border '2:0L@.7(21cm,0cm)+1L@.7(21cm,14.85cm)' $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
    }

    #   only 4 onto 1
    if (   $option{'4page'}         eq 'true'
        && $option{'landscapemode'} eq 'false') {
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSTOPS -q $border '4:0@.49(0cm,14.85cm)+1@.49(10.5cm,14.85cm)+2@.49(0cm,0cm)+3@.49(10.5cm,0cm)' $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
    }

    #   heavy landscape
    if (   $option{'4page'}         eq 'true' 
        && $option{'landscapemode'} eq 'true') {
    # 4:0@.49(0cm,14.85cm)+1@.49(10.5cm,14.85cm)+2@.49(0cm,0cm)+3@.49(10.5cm,0cm)
            $outfile = &File::Tmpfile();
            unlink($outfile);
            &Util::System("$Config::PRG_PSTOPS -q $border '4:0L@.35(10.5cm,0cm)+1L@.35(10.5cm,7.425cm)+2L@.35(21cm,0cm)+3L@.35(21cm,7.425cm)' $file $outfile 2>/dev/null");
            unlink($file) if ($filetype eq 'temporary');
            $file = $outfile;
            $filetype='temporary';
    }

    &printRAW(*option, $file, $filetype);
}

sub printRAW {
    local(*option, $file, $filetype) = @_;

    &Util::Dbg("filter: RAW");

    if ($option{'preview'} eq 'true') {
        if ($Config::HAVE_GHOSTVIEW) {
            &Util::System("$Config::PRG_GHOSTVIEW $file");
            if (&Term::YesPrompt("Want to output this file", "y") == 0) {
                print STDERR "**SKIPPED**\n";
                return;
            }
        }
        else {
            &Util::Warn("Cannot preview Postscript output due to lacking Ghostview");
        }
    }

    if ($option{'outputfile'} ne 'unset') {
        open(FPI, "<$file");
        unlink($option{'outputfile'});
        if (open(FPO, ">$option{'outputfile'}") == 0) {
            &Util::Error("Cannot write outputfile '" . $option{'outputfile'} . "'"); 
        }
        while (<FPI>) {
            print FPO $_;
        }
        close(FPO);
        close(FPI);
        unlink($file) if ($filetype eq 'temporary');
    }
    else {
        $printopt = '';
        if ($Config::USE_SYSV_LP) {
            $printcmd = "lp 2>&1 >/dev/null";
            $printopt .= " -d$option{'printer'}";
        }
        else {
            $printcmd = "lpr";
            $printopt .= " -P$option{'printer'}";
        }
        foreach ($i = 0; $i < $option{'copies'}; $i++) {
            if ($filetype eq 'original') {
                &Util::System("$printcmd $printopt $file");
            }
            else {
                &Util::System("$printcmd $printopt <$file");
                unlink($file);
            }
        }
    }

}

1;

##EOF##
