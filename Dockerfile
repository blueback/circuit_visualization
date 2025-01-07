# Modeline {
#	 vi: foldmarker={,} foldmethod=marker foldlevel=0 tabstop=4 filetype=sh
# }
FROM gcc:latest

RUN apt-get -y update && apt-get install -y

RUN apt-get -y install build-essential

RUN apt-get -y install gdb

RUN apt-get -y install make cmake 

RUN apt-get -y install net-tools

RUN apt-get -y install graphviz

RUN apt-get -y install lcov

RUN apt-get -y install imagemagick

RUN apt-get -y install poppler-utils

RUN apt-get -y install python3-full

RUN apt-get -y install python3-pip

RUN pip3 install pdf2image --break-system-packages

RUN pip3 install img2pdf --break-system-packages

RUN pip3 install pillow --break-system-packages

RUN pip3 install jupyterlab --break-system-packages

RUN pip3 install matplotlib --break-system-packages


# Raylib dependencies {

RUN apt-get -y install libasound2-dev libx11-dev libxrandr-dev libxi-dev
RUN apt-get -y install libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev 
RUN apt-get -y install libxinerama-dev libwayland-dev libxkbcommon-dev

# }


# TurboVNC dependencies {

# Install window managers {

RUN apt-get -y install lxde
RUN apt-get -y install xfce4

# }

RUN apt-get -y install libpam-dev
RUN apt-get -y install libssl-dev

# }

RUN apt-get -y install ffmpeg


# Install lshw to display graphics devices {
#############
# usage is lshw -C DISPLAY
#
RUN apt-get -y install lshw
# }

# Install Mesa vulkan drivers {

############
# following is needed to run glxinfo
#
RUN apt-get -y install mesa-utils

RUN apt-get -y install mesa-vulkan-drivers

# }

# Install Vulkan Packages {

RUN apt-get -y install vulkan-tools

# }

#RUN apt-get -y install gcc g++ cmake

##################################################################
# Workdir is mounted at this location on the container
# putting working_dir in compose.yaml instead of here.
#
#WORKDIR /usr/src/circuit_visualization


##################################################################
# Rather that running from here we run in the container
#
#RUN mkdir build; cd build; cmake ..; cmake --build .;

##################################################################
# Disabling direct run and exit(instead keep live so commented)
#
#CMD [ "/usr/bin/bash" ]
