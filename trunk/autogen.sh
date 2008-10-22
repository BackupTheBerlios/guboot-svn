#!/bin/sh
echo "Running libtoolize"
libtoolize --force
echo "Running aclocal"
aclocal
echo "Running autoheader"
autoheader
echo "Running automake"
automake --add-missing --copy
echo "Running autoconf"
autoconf
echo "All automatic stuff completed: Now you can run configure!"
