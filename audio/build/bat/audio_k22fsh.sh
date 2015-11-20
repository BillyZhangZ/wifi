#!/bin/sh

# expect forward slash paths
ROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"


# copy common files
mkdir -p "${OUTPUTDIR}"
mkdir -p "${OUTPUTDIR}/"
cp -f "${ROOTDIR}/audio/include/cci_metadata.h"    "${OUTPUTDIR}/cci_metadata.h"
cp -f "${ROOTDIR}/audio/include/codec_interface.h" "${OUTPUTDIR}/codec_interface.h"
cp -f "${ROOTDIR}/audio/include/codec_interface_ap_unusedi.h"  "${OUTPUTDIR}/codec_interface_ap_unusedi.h"
cp -f "${ROOTDIR}/audio/include/codec_interface_mqx_wrapper.h" "${OUTPUTDIR}/codec_interface_mqx_wrapper.h"
cp -f "${ROOTDIR}/audio/include/codec_interface_public_api.h"  "${OUTPUTDIR}/codec_interface_public_api.h"
cp -f "${ROOTDIR}/audio/include/codec_support.h"               "${OUTPUTDIR}/codec_support.h"
cp -f "${ROOTDIR}/audio/include/error.h"                       "${OUTPUTDIR}/error.h"
cp -f "${ROOTDIR}/audio/include/errordefs.h"                   "${OUTPUTDIR}/errordefs.h"
cp -f "${ROOTDIR}/audio/include/types.h"                       "${OUTPUTDIR}/types.h"
cp -f "${ROOTDIR}/audio/lib/gcc/codec_metadata.a"              "${OUTPUTDIR}/codec_metadata.a"
cp -f "${ROOTDIR}/audio/lib/gcc/flacdec_lib.a"                 "${OUTPUTDIR}/flacdec_lib.a"
cp -f "${ROOTDIR}/audio/lib/gcc/apedec_lib.a"                  "${OUTPUTDIR}/apedec_lib.a"
cp -f "${ROOTDIR}/audio/lib/gcc/mp3dec_lib.a"                  "${OUTPUTDIR}/mp3dec_lib.a"
cp -f "${ROOTDIR}/audio/lib/gcc/wavdec_lib.a"                  "${OUTPUTDIR}/wavdec_lib.a"

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


