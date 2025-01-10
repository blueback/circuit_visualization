#!/bin/bash

#rm -rf /root/.Xsession
#echo "#!/bin/sh" >> /root/.Xsession
#echo "exec /etc/X11/Xsession startlxde" >> /root/.Xsession

export XORG_CONFIG_FILE=/etc/X11/xorg.conf.d/20-intel.conf
touch ${XORG_CONFIG_FILE}
echo "Section \"Device\"" > ${XORG_CONFIG_FILE}
echo "    Identifier \"Intel Graphics\"" >> ${XORG_CONFIG_FILE}
echo "    Driver \"intel\"" >> ${XORG_CONFIG_FILE}
echo "    Option \"DRI\" \"3\"" >> ${XORG_CONFIG_FILE}
echo "EndSection" >> ${XORG_CONFIG_FILE}

THIRDPARTY_ROOT=${PWD}

export USER="root"
export LD_LIBRARY_PATH=${THIRDPARTY_ROOT}/libjpeg-turbo/build:${LD_LIBRARY_PATH}
export DISPLAY=:1
export VGL_DISPLAY=:1
${THIRDPARTY_ROOT}/turbovnc/build/bin/vncserver :1 -geometry 1920x1080 -rfbport 5902
