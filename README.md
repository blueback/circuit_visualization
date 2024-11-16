<h1>
Circuit Vis
</h1>

Steps to start docker container:
--------------------------------
1. Start the docker rootless using following 

    dockerd-rootless-setup.sh install
    docker compose up -d

2. enter through bash

    docker exec -it circuit_visualization_run

3. build inside container using:

    ./configure.sh

4. stop docker

    docker compose down
    dockerd-rootless-setup.sh uninstall


If you want to directly build it without entering the container:
----------------------------------------------------------------

after creating container using:

    dockerd-rootless-setup.sh install
    docker compose up -d

run:-

    docker exec circuit_visualization_run ./configure.sh

then delete container using:

    docker compose down
    dockerd-rootless-setup.sh uninstall


TODO list:
----------
- modularize inline decisions.
- modularize assert decisions.
