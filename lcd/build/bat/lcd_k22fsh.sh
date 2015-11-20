#!/bin/sh

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/lcd/source/lcd.h" "${OUTPUTDIR}/lcd.h"


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


