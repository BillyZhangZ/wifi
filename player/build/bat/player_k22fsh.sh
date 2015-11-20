#!/bin/sh

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/player/source/play.h"          "${OUTPUTDIR}/play.h"
cp -f "${ROOTDIR}/player/source/lp_main.h"       "${OUTPUTDIR}/lp_main.h"

# iar files
if [ "${TOOL}" = "iar" ]; then
:
fi

# uv4 files
if [ "${TOOL}" = "uv4" ]; then
:
fi

# gcc_arm files
if [ "${TOOL}" = "gcc_arm" ]; then
:
fi


