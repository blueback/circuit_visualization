#!/bin/bash

#rm -rf /root/.Xsession
#echo "#!/bin/sh" >> /root/.Xsession
#echo "exec /etc/X11/Xsession startlxde" >> /root/.Xsession

THIRDPARTY_ROOT=${PWD}

${THIRDPARTY_ROOT}/turbovnc/build/bin/vncserver -kill :1
unset USER
unset DISPLAY
unset VGL_DISPLAY
