# Building: docker build --pull --rm -f "Dockerfile" -t libfxtract:latest "." --network=host
# Running: docker run --rm -it  libfxtract:latest
#           docker run --name frec --network fxtract-network -p 8080:8080 libfxtract:latest
# OR: docker run -itd  --network=fxtract-network libfxtract:latest

# RabbitMQ: docker run -d --rm --net fxtract-network --hostname fxtract_host --name rabbitmq -p 15672:15672 -p 5672:5672 rabbitmq:latest
#       docker run --rm -d  -p 15671:15671/tcp -p 15672:15672/tcp -p 15691:15691/tcp -p 15692:15692/tcp -p 25672:25672/tcp -p 4369:4369/tcp -p 5671:5671/tcp -p 5672:5672/tcp rabbitmq:3.8.16-management
# MongoDB: docker run -d --name mongodb -p 27017:27017 rabbitmq:latest

FROM frolvlad/alpine-gxx AS builder

# Install dependences
RUN apk add --no-cache make cmake \
    build-base \
    boost boost-dev \
    glew-dev \
    openssl openssl-dev graphicsmagick tcl \
    freetype \
    tcl-dev freetype-dev tk tk-dev libevent-dev git \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/nlohmann/json.git \
    && cd json && mkdir -p build \
    && cd build \
    && cmake .. \
    && cmake --build . --target install \
    && rm -rf ../../json

RUN git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git \
    && cd AMQP-CPP && mkdir -p build \
    && cd build \
    && cmake .. -DAMQP-CPP_BUILD_SHARED=ON -DAMQP-CPP_LINUX_TCP=ON \
    && cmake --build . --target install \
    && rm -rf ../../AMQP-CPP


WORKDIR /opt/
COPY ./third_party/OCCT /opt/
RUN mkdir -p build \
    && cd build \
    && cmake .. \
    && cmake --build . --target install \
    && rm -rf ../*

RUN apk add --no-cache make cmake libev-dev

WORKDIR /src
COPY . ./
RUN mkdir -p build \
    && cd build \
    && cmake .. \
    && make \
    && make install

CMD ["fxtract"]


# FROM frolvlad/alpine-gxx

# WORKDIR /usr/bin

# COPY --from=builder /src/build/fxtract /usr/bin/

# CMD ["/opt/fxtract"]




# FROM cpp-build-base:0.1.0 AS build

# WORKDIR /src

# COPY CMakeLists.txt main.cpp ./

# RUN cmake . && make

# FROM ubuntu:bionic

# WORKDIR /opt/hello-world

# COPY --from=build /src/helloworld ./

# CMD ["./helloworld"]

#     && make clean \
#     && rm -rf *

# RUN mkdir WtApp && cd WtApp
# WORKDIR /Fxtract
# COPY ./* /Fxtract/
# RUN mkdir -p build \
#     && cd build \
#     && cmake .. \
#     && make 

# RUN mkdir -p third_party \
#     && cd third_party \
#     && git clone https://github.com/Open-Cascade-SAS/OCCT.git \
#     && ls \
#     && cd OCCT \
#     && mkdir -p build \
#     && cd build \
#     && cmake .. \
#     && make \
#     && make install \
#     && make clean


# COPY ./third_party/wt /opt
# RUN git submodule update --init --recursive
# RUN apk add --no-cache boost-dev
# RUN cd wt && mkdir build && cd build && cmake ../ && make && make install && make clean
# RUN rm -rf wt
# These commands copy your files into the specified directory in the image
# and set that as the working location
# COPY ./third_party/opencascade-7.4.0.tgz /opt/
# WORKDIR /opt/docker_test
# COPY ./hello-docker.cpp /opt/docker_test
# COPY ./Makefile /opt/docker_test
# COPY ./CMakeLists.txt /opt/docker_test

# This command compiles your app using GCC, adjust for your source code
# RUN g++ -o hello-docker hello-docker.cpp -lwthttp -lwt

# This command runs your application, comment out this line to compile only
# CMD ["./hello-docker"]

# LABEL Name=hellogrpc Version=0.0.1

# docker build --pull --rm -f "Dockerfile" -t libfxtract:latest "." --network=host