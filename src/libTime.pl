##
##  libTime.pl -- Time functions
##  Copyright (c) Ralf S. Engelschall, All Rights Reserved.
##

package Time;

#   Use this function to convert a time() output to a "SUN (13.03.94)"
#   representation.
#
#   Usage: $str = &givetime(time());
#
@dow = ( 'Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat' );
@moy = ( 'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
         'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' );

sub ctime {
    local($time) = $_[0];
    local($str);

    local($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) =
        localtime($time);
    $str = sprintf("%s %s %2d %02d:%02d:%02d 19%s%s", 
        $dow[$wday], $moy[$mon], $mday, $hour, $min, $sec, $year, 
        $isdst ? " DST" : "");
    return $str;
}

1;

##EOF##
