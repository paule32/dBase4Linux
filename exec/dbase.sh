#!/bin/sh
#----------------------------------------
# (c) 2016 by Jens Kallup
#----------------------------------------

appname=`basename $0 | sed s,\.sh$,,`
dirname=`dirname $0`

tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

export LD_LIBRARY_PATH="$dirname./lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="./:/opt/qt5/5.5/gcc_64/lib:$LD_LIBRARY_PATH"
export QT_QPA_PLATFORM_PLUGIN_PATH="/opt/qt5/5.5/gcc_64/plugins"

gdb $dirname/$appname "$@"
