#!/bin/sh
#
# Create the version.cpp file

cat <<EOF >version.cpp
/* Automatically generated file. Do not edit */

#include <string>

using namespace std;

#include "version.h"

char Version::revision[] = "$(git log -n 1 --format='%H' || echo UNKNOWN)";
char Version::date[] = "$(git log -n 1 --format='%ci' || echo UNKNOWN)";
char Version::compiled[] = "$(date +'%F %T')";
EOF
