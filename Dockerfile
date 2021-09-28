# We use Ubuntu image as OpenCascade (at least 7.4.0 won't compile on alpine).
FROM ubuntu:latest

# To avoid all sort of prompts from apt-get.
# You use this mode when you need zero interaction while installing or upgrading the system via apt.
# It accepts the default answer for all questions. It might mail an error message to the root user,
# but that's it all. Otherwise, it is totally silent and humble, a perfect frontend for automatic
# installs. One can use such mode in Dockerfile, shell scripts, cloud-init script, and more.
ENV DEBIAN_FRONTEND=noninteractive

# The build-essentials packages are meta-packages that are necessary for compiling software.
# They include the GNU debugger, g++/GNU compiler collection, and some more tools and libraries
# that are required to compile a program. For example, if you need to work on a C/C++ compiler,
# you need to install essential meta-packages on your system before starting the C compiler installation.
# When installing the build-essential packages, some other packages such as G++, dpkg-dev, GCC and make, etc.
# also install on your system.
RUN apt-get update --fix-missing && \
    apt-get -y install build-essential git cmake libboost-all-dev libev-dev \
    openssl libssl*-dev tcl tcl-dev tk tk-dev libfreeimage-dev libxmu-dev libxi-dev \
    libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev xvfb wget && \
    apt-get clean all && \
    rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/nlohmann/json.git \
    && cd json && mkdir -p build \
    && cd build \
    && cmake .. \
    && cmake --build . --target install \
    && cmake --build . --target clean \
    && rm -rf ../../json

RUN git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git \
    && cd AMQP-CPP && mkdir -p build \
    && cd build \
    && cmake .. -DAMQP-CPP_BUILD_SHARED=ON -DAMQP-CPP_LINUX_TCP=ON \
    && cmake --build . --target install \
    && cmake --build . --target clean \
    && rm -rf ../../AMQP-CPP

# RUN wget https://github.com/tpaviot/oce/releases/download/official-upstream-packages/opencascade-7.4.0.tgz \
#     && tar -xzf opencascade-7.4.0.tgz \
#     && cd opencascade-7.4.0/ && mkdir -p build \
#     && cd build \
#     && cmake .. \
#     && cmake --build . --target install 

RUN git clone https://github.com/Open-Cascade-SAS/OCCT.git \
    && cd OCCT \
    && mkdir -p build \
    && cd build \
    && cmake .. \
    -DCMAKE_BUILD_TYPE=release \
    && make \
    && make install \
    && make clean

WORKDIR /app
COPY . .
RUN export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib && mkdir -p build \
    && cd build \
    && cmake .. \
    && make \
    && make install

CMD ["fre-service"]
