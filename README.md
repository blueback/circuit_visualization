<h1 align="center">
Circuit Visualization
</h1>

Steps to start docker container
-------------------------------
1. Start rootless docker deamon:-

        dockerd-rootless-setup.sh install

2. Start docker container:-

        docker compose up -d

    a. To build fresh while creating container

        docker compose up -d --build

3. Enter container:-

        docker exec -it circuit_visualization_run su root

Steps to start VNC-Raylib
-------------------------
1. Download and build raylib/turboVNC:-

        cd third_party
        ./build_vnc_raylib.sh
        cd ..

2. Start vncserver at port 5902:-

        cd third_party
        source vnc_start.sh
        cd ..

3. Run raylib example:-

        cd third_party/raylib/build/examples
        ./core_3d_camera_first_person
        cd ../../../..

4. Install raylib inside third_party:-

        cd third_party/raylib/build
        DESTDIR=../../. make install
        cd ../../..

    The distribution will then be installed at:

        find third_party/usr/local

To build circuit visualization applicaiton
------------------------------------------
Build application inside container using:

        ./configure.sh

Steps to stop VNC
-----------------
1. Stop vncserver:-

        cd third_party
        source vnc_stop.sh
        cd ..

Steps to stop docker container
-------------------------------
1. Stop docker container:-

        docker compose down

2. Stop docker deamon:-

        dockerd-rootless-setup.sh uninstall

3. To delete the container for good!:-

        docker rm circuit_visualization_run

NOTES:
-----------------------
1. To enable TCP port 5902, run these in the host machine

        sudo ufw allow 5901/tcp
        sudo ufw status

2. To enable device graphics driver access to docker:-

        sudo chmod 666 /dev/dri/*

TODO list:
----------
- modularize inline decisions.
- modularize assert decisions.

Acknowledgement:
----------------
1. Video rendering code, which uses ffmpeg is inspired by tsoding daily(zezin).
