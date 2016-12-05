#!/bin/bash

old_dir="/dbase/source/debug"
echo $old_dir

new_dir="/dbase/source/build-debug-KALLUP_g_5_4_0-Debug"
rm -rf $new_dir/debug.hlp
rm -rf $new_dir/debug.h

$old_dir/tvhc $old_dir/debughelp.txt $new_dir/debug.hlp $new_dir/debug.h

cd $old_dir

echo "done."
