#!/bin/sh

apxs -c /home/bak/src/ui/dbase/dBase/src/compiler/source/apache2DSO/mod_dbase.c
cp /home/bak/src/ui/dbase/dBase/src/compiler/source/apache2DSO//apache2DSO/.libs/mod_dbase.o /var/www/html/dbase/mod_dbase.o
