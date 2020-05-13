#!/bin/bash

TERSE="false"

OUT_MOOS_LIBS_ONLY="no"
OUT_MOOS_APPS_ONLY="no"
OUT_IVP_LIBS_ONLY="no"
OUT_IVP_APPS_ONLY="no"

MISSING_MOOS_LIBS=""
MISSING_MOOS_APPS=""
MISSING_IVP_LIBS=""
MISSING_IVP_APPS=""

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        echo "build-check.sh [SWITCHES]                      "
        echo "  --help, -h                                   "
        echo "  --terse,-t   Output terse report             "
        echo "  --mooslib    Only show missing MOOS libs     "
        echo "  --moosapp    Only show missing MOOS apps     "
        echo "  --ivplib     Only show missing IvP libs      "
        echo "  --ivpapp     Only show missing IvP apps      "
        echo "                                               "
        echo "Examples:                                      "
        echo "  build-check.sh                               "
        echo "  build-check.sh --terse                       "
        echo "  build-check.sh --mooslib                     "
        echo "  build-check.sh --moosapp                     "
        echo "  build-check.sh --ivplib                      "
        echo "  build-check.sh --ivpapp                      "
        exit 0;
    elif [ "${ARGI}" = "--mooslib" ] ; then
        OUT_MOOS_LIBS_ONLY="yes"
    elif [ "${ARGI}" = "--moosapp" ] ; then
        OUT_MOOS_APPS_ONLY="yes"
    elif [ "${ARGI}" = "--ivplib" ] ; then
        OUT_IVP_LIBS_ONLY="yes"
    elif [ "${ARGI}" = "--ivpapp" ] ; then
        OUT_IVP_APPS_ONLY="yes"
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ] ; then
        TERSE="true"
    fi
done

INVOC_ABS_DIR="$(pwd)"
SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"

cd "${SCRIPT_ABS_DIR}"

#================================================================
# Part: Check existence of all MOOS Libraries
#================================================================
MISSING=""

if [ "`uname`" == "Darwin" ] ; then
    if [ ! -e MOOS/MOOSGeodesy/lib/libMOOSGeodesy.dylib ]; then
	MISSING=$MISSING"MOOSGeodesy,";
    fi
    if [ ! -e MOOS/MOOSToolsUI/lib/libfltkvw.dylib ]; then
	MISSING=$MISSING"fltkvw,";
    fi
else
    if [ ! -e MOOS/MOOSGeodesy/lib/libMOOSGeodesy.so ]; then
	MISSING=$MISSING"MOOSGeodesy,";
    fi
    if [ ! -e MOOS/MOOSToolsUI/lib/libfltkvw.so ]; then
	MISSING=$MISSING"fltkvw,";
    fi
fi

if [ ! -e MOOS/MOOSCore/lib/libMOOS.a ]; then
    MISSING=$MISSING"MOOS,";
fi
if [ ! -e MOOS/MOOSGeodesy/lib/libproj.a ]; then
    MISSING=$MISSING"proj,";
fi

if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_MOOS_LIBS="${MISSING%?}";
fi

#=================================================================
# Part 2: Check health of MOOS Apps
#=================================================================
MISSING=""

if [ ! -e bin/umm ];         then  MISSING=$MISSING"umm,"; fi
if [ ! -e bin/ktm ];         then  MISSING=$MISSING"ktm,"; fi
if [ ! -e bin/mtm ];         then  MISSING=$MISSING"mtm,"; fi
if [ ! -e bin/mqos ];        then  MISSING=$MISSING"mqos,"; fi
if [ ! -e bin/pMOOSBridge ]; then  MISSING=$MISSING"pMOOSBridge,"; fi
if [ ! -e bin/pAntler ];     then  MISSING=$MISSING"pAntler,"; fi
if [ ! -e bin/pLogger ];     then  MISSING=$MISSING"pLogger,"; fi
if [ ! -e bin/pScheduler ];  then  MISSING=$MISSING"pScheduler,"; fi
if [ ! -e bin/pShare ];      then  MISSING=$MISSING"pShare,"; fi
if [ ! -e bin/uMS ];         then  MISSING=$MISSING"uMS,"; fi
if [ ! -e bin/uPlayback ];   then  MISSING=$MISSING"uPlayback,"; fi
if [ ! -e bin/iRemoteLite ]; then  MISSING=$MISSING"iRemoteLite,"; fi
if [ ! -e bin/uPoke ];       then  MISSING=$MISSING"uPoke,"; fi

if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_MOOS_APPS="${MISSING%?}";
fi

#=================================================================
# Part 3: Check health of IvP Libraries
#=================================================================
MISSING=""

if [ ! -e lib/libapputil.a ];       then  MISSING=$MISSING"apputil,"; fi
if [ ! -e lib/libbehaviors.a ];     then  MISSING=$MISSING"behaviors,"; fi
if [ ! -e lib/libbehaviors-marine.a ];  then  MISSING=$MISSING"behaviors-marine,"; fi
if [ ! -e lib/libbhvutil.a ];       then  MISSING=$MISSING"bhvutil,"; fi
if [ ! -e lib/libcontacts.a ];      then  MISSING=$MISSING"contacts,"; fi
if [ ! -e lib/libgenutil.a ];       then  MISSING=$MISSING"genutil,"; fi
if [ ! -e lib/libgeometry.a ];      then  MISSING=$MISSING"geometry,"; fi

if [ ! -e lib/libhelmivp.a ];       then  MISSING=$MISSING"helmivp,"; fi
#if [ ! -e lib/libhenrik_anneal.a ];     then  MISSING=$MISSING"henrik_anneal,"; fi
if [ ! -e lib/libipfview.a ];       then  MISSING=$MISSING"ipfview,"; fi
if [ ! -e lib/libivpbuild.a ];      then  MISSING=$MISSING"ivpbuild,"; fi
if [ ! -e lib/libivpcore.a ];       then  MISSING=$MISSING"ivpcore,"; fi

if [ ! -e lib/libivpsolve.a ];      then  MISSING=$MISSING"ivpsolve,"; fi
if [ ! -e lib/liblogic.a ];         then  MISSING=$MISSING"logic,"; fi
if [ ! -e lib/liblogutils.a ];      then  MISSING=$MISSING"logutils,"; fi
if [ ! -e lib/libmarineview.a ];    then  MISSING=$MISSING"marineview,"; fi
if [ ! -e lib/libmbutil.a ];        then  MISSING=$MISSING"mbutil,"; fi

if [ ! -e lib/libufield.a ];       then  MISSING=$MISSING"ufield,"; fi
if [ ! -e lib/libufld_hazards.a ]; then  MISSING=$MISSING"ufld_hazards,"; fi

if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_IVP_LIBS="${MISSING%?}";
fi

#=================================================================
# Part 4: Check health of IvP Apps
#=================================================================
MISSING=""

if [ ! -e bin/alogcheck ];   then  MISSING=$MISSING"alogcheck,"; fi
if [ ! -e bin/alogclip ];    then  MISSING=$MISSING"alogclip,"; fi
if [ ! -e bin/aloggrep ];    then  MISSING=$MISSING"aloggrep,"; fi
if [ ! -e bin/aloghelm ];    then  MISSING=$MISSING"aloghelm,"; fi
if [ ! -e bin/alogiter ];    then  MISSING=$MISSING"alogiter,"; fi

if [ ! -e bin/alogrm ];      then  MISSING=$MISSING"alogrm,"; fi
if [ ! -e bin/alogscan ];    then  MISSING=$MISSING"alogscan,"; fi
if [ ! -e bin/alogsort ];    then  MISSING=$MISSING"alogsort,"; fi
if [ ! -e bin/alogview ];    then  MISSING=$MISSING"alogview,"; fi

if [ ! -e bin/ffview ];        then  MISSING=$MISSING"ffview,"; fi
if [ ! -e bin/gen_hazards ];   then  MISSING=$MISSING"gen_hazards,"; fi
if [ ! -e bin/geoview ];       then  MISSING=$MISSING"geoview,"; fi
if [ ! -e bin/nsplug ];        then  MISSING=$MISSING"nsplug,"; fi


if [ ! -e bin/gzaicview ];     then  MISSING=$MISSING"gzaicview,"; fi
if [ ! -e bin/vzaicview ];     then  MISSING=$MISSING"vzaicview,"; fi

if [ ! -e bin/iSay ];          then  MISSING=$MISSING"iSay,"; fi
if [ ! -e bin/pBasicContactMgr ]; then  MISSING=$MISSING"pBasicContactMgr,"; fi
if [ ! -e bin/pEchoVar ];      then  MISSING=$MISSING"pEchoVar,"; fi

if [ ! -e bin/pHelmIvP ];      then  MISSING=$MISSING"pHelmIvP,"; fi
if [ ! -e bin/pHostInfo ];     then  MISSING=$MISSING"pHostInfo,"; fi
if [ ! -e bin/pMarinePID ];    then  MISSING=$MISSING"pMarinePID,"; fi
if [ ! -e bin/pMarineViewer ]; then  MISSING=$MISSING"pMarineViewer,"; fi

if [ ! -e bin/pNodeReporter ]; then  MISSING=$MISSING"pNodeReporter,"; fi
if [ ! -e bin/pSearchGrid ];   then  MISSING=$MISSING"pSearchGrid,"; fi
if [ ! -e bin/uFldWrapDetect ];   then  MISSING=$MISSING"uFldWrapDetect,"; fi
if [ ! -e bin/pXRelay ];       then  MISSING=$MISSING"pXRelay,"; fi

if [ ! -e bin/uFldBeaconRangeSensor ]; then  MISSING=$MISSING"uFldBeaconRangeSensor,"; fi
if [ ! -e bin/uFldCollisionDetect ]; then  MISSING=$MISSING"uFldCollisionDetect,"; fi
if [ ! -e bin/uFldContactRangeSensor ]; then  MISSING=$MISSING"uFldContactRangeSensor,"; fi

if [ ! -e bin/uFldGenericSensor ];   then  MISSING=$MISSING"uFldGenericSensor,"; fi
if [ ! -e bin/uFldHazardMetric ];    then  MISSING=$MISSING"uFldHazardMetric,"; fi
if [ ! -e bin/uFldHazardMgr ];       then  MISSING=$MISSING"uFldHazardMgr,"; fi
if [ ! -e bin/uFldHazardSensor ];    then  MISSING=$MISSING"uFldHazardSensor,"; fi

if [ ! -e bin/uFldMessageHandler ];  then  MISSING=$MISSING"uFldMessageHandler,"; fi
if [ ! -e bin/uFldNodeBroker ];      then  MISSING=$MISSING"uFldNodeBroker,"; fi
if [ ! -e bin/uFldNodeComms ];       then  MISSING=$MISSING"uFldNodeComms,"; fi
if [ ! -e bin/uFldPathCheck ];       then  MISSING=$MISSING"uFldPathCheck,"; fi
if [ ! -e bin/uFldScope ];           then  MISSING=$MISSING"uFldScope,"; fi

if [ ! -e bin/uFldShoreBroker ];     then  MISSING=$MISSING"uFldShoreBroker,"; fi
if [ ! -e bin/uFunctionVis ];  then  MISSING=$MISSING"uFunctionVis,"; fi
if [ ! -e bin/uHelmScope ];    then  MISSING=$MISSING"uHelmScope,"; fi
if [ ! -e bin/uLoadWatch ];    then  MISSING=$MISSING"uLoadWatch,"; fi

if [ ! -e bin/uMAC ];          then  MISSING=$MISSING"uMAC,"; fi
if [ ! -e bin/uMACView ];      then  MISSING=$MISSING"uMACView,"; fi
if [ ! -e bin/uPokeDB ];       then  MISSING=$MISSING"uPokeDB,"; fi
if [ ! -e bin/uProcessWatch ]; then  MISSING=$MISSING"uProcessWatch,"; fi

if [ ! -e bin/uSimMarine ];    then  MISSING=$MISSING"uSimMarine,"; fi
if [ ! -e bin/uTermCommand ];  then  MISSING=$MISSING"uTermCommand,"; fi
if [ ! -e bin/uTimerScript ];  then  MISSING=$MISSING"uTimerScript,"; fi
if [ ! -e bin/uXMS ];          then  MISSING=$MISSING"uXMS,"; fi

if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_IVP_APPS="${MISSING%?}";
fi


#===============================================================
# Part 5: Handle when specific output is requested.
#===============================================================

if [ "${OUT_MOOS_LIBS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_MOOS_LIBS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_MOOS_LIBS
	exit 1
    fi
fi

if [ "${OUT_MOOS_APPS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_MOOS_APPS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_MOOS_APPS
	exit 1
    fi
fi

if [ "${OUT_IVP_LIBS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_IVP_LIBS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_IVP_LIBS
	exit 1
    fi
fi

if [ "${OUT_IVP_APPS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_IVP_APPS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_IVP_APPS
	exit 1
    fi
fi



#===============================================================
# Part 6: Handle when collective output is requested.
#===============================================================
if [ "$TERSE" == "false" ]; then
    echo "     Missing MOOS Libs: " $MISSING_MOOS_LIBS
    echo "     Missing MOOS Apps: " $MISSING_MOOS_APPS
    echo "      Missing IvP Libs: " $MISSING_IVP_LIBS
    echo "      Missing IvP Apps: " $MISSING_IVP_APPS
fi
    
if [ "$MISSING_ALL" == "" ]; then
    echo "PASS";
else
    if [[ "$MISSING_ALL" == *, ]]; then
	MISSING_ALL="${MISSING_ALL%?}";
    fi
    echo "FAIL{"$MISSING_ALL"}";
fi



cd "${INVOC_ABS_DIR}"

