<h1 align="center">
Circuit Visualization
</h1>

Steps to start docker container:
--------------------------------
1. Start the docker rootless using following 

        dockerd-rootless-setup.sh install
        docker compose up -d

    a. To build fresh while creating container

        docker compose up -d --build

2. enter through bash

        docker exec -it circuit_visualization_run sh

    or

        docker exec -it circuit_visualization_run su root

3. build inside container using:

        ./configure.sh

4. stop docker

        docker compose down
        dockerd-rootless-setup.sh uninstall

5. To delete the container for good!:-

        docker rm circuit_visualization_run


If you want to directly build it without entering the container:
----------------------------------------------------------------

1. after creating container using:

        dockerd-rootless-setup.sh install
        docker compose up -d

2. run:-

        docker exec circuit_visualization_run ./configure.sh

3. then delete container using:

        docker compose down
        dockerd-rootless-setup.sh uninstall


TODO list:
----------
- modularize inline decisions.
- modularize assert decisions.
