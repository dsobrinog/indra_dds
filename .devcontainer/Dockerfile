FROM ubuntu:25.04

ARG DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

# Install build tools
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        g++ \
        cmake \
        gdb \
        pkg-config \
        git \
#FastDDS
    wget \
    libasio-dev \
    libtinyxml2-dev \
#FastDDS-Gen
    openjdk-11-jdk 


# Default command
CMD [ "bash" ]

# Set working directory
WORKDIR /workspace