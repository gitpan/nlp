##
##  modQueue.pl -- Printer Queue Support
##  Copyright (c) Ralf S. Engelschall, All Rights Reserved.
##

package Queue;

sub queue {
   local(*option) = @_;

   if ($Config::USE_SYSV_LP) {
      if ($Config::host_os =~ m|solaris.*|) {
          system("lpq -P$option{'printer'}");
      }
      else {
          system("lpstat -p $option{'printer'}"); ## ?????? NOT SURE ????
      }
   }
   else {
      system("lpq -P$option{'printer'}");
   }
}

sub queueall {
   local(*option) = @_;

    foreach $printer (sort(keys(%Cap::CapAlias))) {
       if ($Config::USE_SYSV_LP) {
         if ($Config::host_os =~ m|solaris.*|) {
             system("lpq -P$printer");
         }
         else {
             system("lpstat -p $printer"); ## ?????? NOT SURE ????
         }
      }
      else {
         system("lpq -P$printer");
      }
   }

}

1;

##EOF##
