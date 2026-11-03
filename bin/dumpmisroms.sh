#!/bin/sh

rm misroms.sql

$FIREBIRD/bin/isql -quiet -u SYSDBA -p masterkey -i dumpmisroms.sql -o misroms.sql misroms.fdb

