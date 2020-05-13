#!/bin/bash 
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
VERBOSE=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "clean.sh [SWITCHES]                "
	echo "  --verbose                        " 
	echo "  --help, -h                       " 
	exit 0;	
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="-v"
    else 
	echo "clean.sh: Bad Arg:" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------
if [ "${VERBOSE}" = "-v" ]; then
    echo "Cleaning: $PWD"
fi
rm -rf  $VERBOSE   MOOSLog_*  LOG_* 
rm -f   $VERBOSE   *~  *.moos++
rm -f   $VERBOSE   .LastOpenedMOOSLogDirectory