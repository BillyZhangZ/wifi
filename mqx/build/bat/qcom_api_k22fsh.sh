#!/bin/sh

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
mkdir -p "${OUTPUTDIR}/.."
mkdir -p "${OUTPUTDIR}/Generated_Code"
