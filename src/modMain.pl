##
##  modMain.pl -- Main procedure
##  Copyright (c) Ralf S. Engelschall, All Rights Reserved.
##

package Main;

#
#   global usage
#
sub usage {
    $printer = "UNKNOWN";
    if ($ENV{'PRINTER'} ne '') {
       $printer = $ENV{'PRINTER'};
    }
print STDERR <<"EOT"
Usage: $prgname [options] file [[options} file] ...

 Input Options:                       Output Options:                          
 r    remove print mode header        c #  print multiple copies (def: 1)
 f s  apply external filter           p    preview output before printing     
                                    P,d s  select printer (def: $printer)    
 ASCII to Postscript Options:         o s  write to file instead of printer  
 F #  fontsize in points                                                     
 B    use bold font                   Printer Options:                       
 t #  tabindent (def: 8)              D    force duplex  mode                
 w    wrap/fold lines if too long     S    force simplex mode                
 l #  lines per page (adj.fontsize)   T #  select papertray                  
 N    line-numbers                    M    manual paper input                
 H s  page header                                                            
 m    visualize metachars TAB,BS,FF   Special Command Options:               
                                      q    display queue of specific printer 
 DVI to Postscript Options:           Q    display queue of all printers     
 R #  resolution in DPI (def: 300)    i    information about specific printer
                                      I    information about all printers    
 Common Layout Options:               n    notify admin about printer trouble
 L    landscape mode (def: portrait)  h    this page                         
 2    two pages on one physical       V    version information               
 4    four pages on one physical                                             
 s r  print only selected pages       General Processing Options:            
 b    draw surrounding borders        v    verbose mode                      
                                      e    expert mode                       
 #=number                             x    debug mode                        
 f=file                             
 s=string                          
 r=range (#,-#,#-,#-#,r,o,e,S,B)  
EOT
}


sub Main {
    #   get dirname and basename of program
    $prgpath = $0;
    if ($prgpath =~ m|^[^/]+$|) {
        $prgpath = ".";
    }
    else {
        $prgpath =~ s|^(.*)/[^/]+$|$1|;
    }
    $prgname = $0;
    $prgname =~ s|^.*/([^/]+)$|$1|;
    
    #   common definitions
    $tmpdir = "/tmp";

    #   argument line parsing
    %optiondecl = (
        'removeheader',  "r  false",
        'filter',        "f: unset",
        'fontsize',      "F: unset",
        'boldface',      "B  false",
        'borders',       "b  false",
        'tabindent',     "t: 8",
        'wraplines',     "w  false",
        'maxlines',      "l: unset",
        'linenumbers',   "N  false",
        'pageheader',    "H: unset",
        'metachars',     "m  false",
        'dpi',           "R: 300",
        'landscapemode', "L  false",
        '2page',         "2  false",
        '4page',         "4  false",
        'selectpage',    "s: unset",
        'copies',        "c: 1",
        'preview',       "p  false",
        'printer',       "P: unset",
        'printerd',      "d: unset",
        'outputfile',    "o: unset",
        'duplexmode',    "D  false",
        'simplexmode',   "S  false",
        'papertray',     "T: unset",
        'manualfeed',    "M  false",
        'queue',         "q  false",
        'queueall',      "Q  false",
        'info',          "i  false",
        'infoall',       "I  false",
        'notify',        "n  false",
        'help',          "h  false",
        'version',       "V  false",
        'debug',         "x  false",
        'verbose',       "v  false",
        'expert',        "e  false"
    );
    $optionlist = '';
    foreach $val (values(%optiondecl)) {
        $val =~ m|^([a-zA-Z0-9][:+]?).*|;
        $optionlist = $optionlist . $1;
    }

    if ($#ARGV eq -1) {
        &usage;
        exit $Util::EX{'OK'};
    }

    &Cap::LoadCap("${Config::libdir}/printer.cap");

    $cnt = 0;
    while ($#ARGV >= 0) {
        $cnt++;

        #  prepend the argline with contents of NLPOPTS
        if ($ENV{'NLPOPTS'} ne '') {
            @ARGVN = split(' ', $ENV{'NLPOPTS'});
            foreach $opt (@ARGV) {
                push(@ARGVN, $opt);
            }
            @ARGV = @ARGVN;
        }

        ($rc, @ARGV) = &Getopts::Getopts($optionlist, @ARGV);
        if ($rc == 0) {
            print STDERR "Type '$prgname -h' for usage.\n";
            exit $Util::EX{'USAGE'};
        }
        foreach $optname (keys(%optiondecl)) {
            $optchar = $optiondecl{$optname};
            $optchar =~ m|^([a-zA-Z0-9])([:+]?)[ \t]+(.+)|;
            ($optchar, $optcharCard, $optdef) = ($1, $2, $3);
            eval "\$optval = \$Getopts::opt_$optchar;";
            if ($optval ne '') {
                if ($optcharCard eq ':') {
                    $option{$optname} = $optval;
                }
                else {
                    $option{$optname} = ($optval == 0 ? "false" : "true");
                }
            }
            else {
                $option{$optname} = $optdef;
            }
            # cleanup for next while(..) iteration
            # i.e. next time don't want to see the options of
            # an old option block
            eval "\$Getopts::opt_$optchar = '';"; 
        }

        #  special commands are processed at any time
        if ($option{'help'} eq 'true') {
            &usage;
            exit $Util::EX{'OK'};
        }
        if ($option{'version'} eq 'true') {
            print STDERR "$Vers::NLP_Hello\n";
            print STDERR "[$Config::target]\n";
            print STDERR "\n";
            print STDERR "  Copyright (c) 1992-1997\n";
            print STDERR "    Ralf S. Engelschall,\n";
            print STDERR "    All Rights Reserved.\n";
            print STDERR "  Copyright (c) 1992-1993 [version 1 only]\n";
            print STDERR "    software design & management (sd&m) GmbH & Co KG,\n";
            print STDERR "    All Rights Reserved.\n";
            print STDERR "  Copyright (c) 1995 [version 2 only]\n";
            print STDERR "    Technische Universitaet Muenchen (TUM),\n";
            print STDERR "    All Rights Reserved.\n";
            print STDERR "\n";
            exit $Util::EX{'OK'};
        }
        if ($option{'queueall'} eq 'true') {
            &Queue::queueall(*option);
            exit $Util::EX{'OK'};
        }
        if ($option{'infoall'} eq 'true') {
            &Info::infoall(*option);
            exit $Util::EX{'OK'};
        }

        #  be careful, the underlying
        if ($option{'pageheader'} ne '' && length($option{'pageheader'}) > 35) {
           print STDERR "Sorry, pageheaders can be max. 35 characters long\n";
           exit $Util::EX{'FAIL'};
        }

        if ($option{'printer'} eq 'unset' && $option{'printerd'} ne 'unset') {
            $option{'printer'} = $option{'printerd'};
        }
        if ($option{'printer'} eq 'unset' && $ENV{'PRINTER'} ne '') {
            $option{'printer'} = $ENV{'PRINTER'};
        }
        if ($option{'printer'} eq 'unset' && $option{'outputfile'} eq 'unset') {
            print STDERR "*** Sorry, you must provide me with a destination printer\n";
            print STDERR "*** by at least having either a PRINTER env-variable or\n";
            print STDERR "*** specifing a explicit printer via -P or -d option!\n";
            exit $Util::EX{'FAIL'};
        }

        if ($option{'queue'} eq 'true') {
            &Queue::queue(*option);
            exit $Util::EX{'OK'};
        }
        if ($option{'info'} eq 'true') {
            &Info::info(*option);
            exit $Util::EX{'OK'};
        }
        if ($option{'notify'} eq 'true') {
            &Notify::notify(*option);
            exit $Util::EX{'OK'};
        }

        if ($#ARGV == -1) {
            print STDERR "*** you have to specify at least one file to print\n";
            exit $Util::EX{'FAIL'};

        }
        @files = ();
        while ($#ARGV >= 0 && $ARGV[0] !~ m|^-.|) {
            push(@files, $ARGV[0]);
            shift(@ARGV);
        }

        eval "\%option${cnt} = \%option;";
        eval "push(\@OPTIONPTR, \*option${cnt});";
        eval "\@files${cnt} = \@files;";
        eval "push(\@FILESPTR, \*files${cnt});";
        
    }

    #
    #  print it!
    #
    for ($j = 0; $j < $cnt; $j++) {
        *option = $OPTIONPTR[$j];
        *files  = $FILESPTR[$j];
        &Print::print(*option, *files);
    }
}

1;

##EOF##
