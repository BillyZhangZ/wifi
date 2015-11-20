CURRENTDIR=$PWD
USBROOTDIR="${1}"
OUTPUTDIR="${2}"
TOOL="${3}"
OS="${4}"


mkdir -p $OUTPUTDIR
cd  $USBROOTDIR

cp -f  usb_core/include/usb.h $OUTPUTDIR
cp -f  usb_core/include/compiler.h $OUTPUTDIR
cp -f  usb_core/include/types.h $OUTPUTDIR
cp -f  usb_core/include/usb_desc.h $OUTPUTDIR
cp -f  usb_core/include/usb_error.h $OUTPUTDIR
cp -f  usb_core/include/usb_misc.h $OUTPUTDIR
cp -f  usb_core/include/usb_opt.h $OUTPUTDIR
cp -f  usb_core/include/usb_types.h $OUTPUTDIR
cp -f  adapter/sources/adapter.h $OUTPUTDIR
cp -f  adapter/sources/adapter_types.h $OUTPUTDIR
cp -f  usb_core/device/include/twrk22f120m/usb_device_config.h $OUTPUTDIR
cp -f  usb_core/device/include/usb_device_stack_interface.h $OUTPUTDIR
cp -f  usb_core/device/sources/classes/include/usb_class_audio.h $OUTPUTDIR
cp -f  usb_core/device/sources/classes/include/usb_class_cdc.h $OUTPUTDIR
cp -f  usb_core/device/sources/classes/include/usb_class.h $OUTPUTDIR
cp -f  usb_core/device/sources/classes/include/usb_class_composite.h $OUTPUTDIR
cp -f  usb_core/device/sources/classes/include/usb_class_hid.h $OUTPUTDIR
cp -f  usb_core/device/sources/classes/include/usb_class_msc.h $OUTPUTDIR
cp -f  usb_core/device/sources/classes/include/usb_class_phdc.h $OUTPUTDIR
cp -f  usb_core/host/include/twrk22f120m/usb_host_config.h $OUTPUTDIR
cp -f  usb_core/host/include/usb_host_stack_interface.h $OUTPUTDIR
cp -f  usb_core/host/sources/classes/hid/usb_host_hid.h $OUTPUTDIR
cp -f  usb_core/host/sources/classes/phdc/usb_host_phdc.h $OUTPUTDIR
cp -f  usb_core/host/sources/classes/hub/usb_host_hub.h $OUTPUTDIR
cp -f  usb_core/host/sources/classes/audio/usb_host_audio.h $OUTPUTDIR
cp -f  usb_core/host/sources/classes/hub/usb_host_hub_sm.h $OUTPUTDIR
cp -f  usb_core/host/sources/classes/msd/usb_host_msd_bo.h $OUTPUTDIR
cp -f  usb_core/host/sources/classes/msd/usb_host_msd_ufi.h $OUTPUTDIR
cp -f  usb_core/otg/sources/include/usb_otg_main.h $OUTPUTDIR

cp -f adapter/sources/mqx/adapter_mqx.h $OUTPUTDIR
cp -f adapter/sources/mqx/adapter_cfg.h $OUTPUTDIR

cd  $CURRENTDIR

