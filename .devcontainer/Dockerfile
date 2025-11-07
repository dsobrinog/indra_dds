FROM ubuntu:25.04

ARG DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

# Install build tools and necessary dependencies
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        g++ \
        cmake \
        gdb \
        pkg-config \
        git \
        wget \
        curl

# Install FastDDS dependencies
RUN apt-get update && \
    apt-get install -y \
        libasio-dev \
        libtinyxml2-dev \
        libssl-dev \
        python3 \
        python3-pip \
        openjdk-11-jdk

ENV LD_LIBRARY_PATH=/workspaces/testdds/lib/Fast-DDS/fastdds-install/lib:$LD_LIBRARY_PATH

# Install CycloneDDS dependencies
RUN apt-get update && \
    apt-get install -y \
        flex \
        bison

# Install OpenDDS dependencies
RUN apt-get update && \
    apt-get install -y \
        perl \
        libssl-dev \
        libxerces-c-dev && \
    rm -rf /var/lib/apt/lists/*

    # Correct paths — using the installed OpenDDS tree
ENV DDS_ROOT=/workspaces/testdds/lib/OpenDDS/OpenDDS
ENV ACE_ROOT=$DDS_ROOT
ENV PATH=$DDS_ROOT/bin:$PATH
ENV LD_LIBRARY_PATH=$DDS_ROOT/lib:$LD_LIBRARY_PATH

# ENV DDS_ROOT=/workspaces/testdds/lib/OpenDDS/OpenDDS
# ENV ACE_ROOT=$DDS_ROOT/ACE_wrappers
# ENV PATH=$DDS_ROOT/bin:$ACE_ROOT/bin:$PATH
# ENV LD_LIBRARY_PATH=$DDS_ROOT/lib:$ACE_ROOT/lib:$LD_LIBRARY_PATH

# ==============================
# Container set up for docker compose
# ==============================
# Set working directory
WORKDIR /workspaces/testdds
    
# Copy project files into the container
# COPY . .

# Build FastDDS application
# RUN mkdir -p build && \
#     cd build && \
#     cmake .. && \
#     make -j$(nproc)

# Ensure launch.sh is executable
# RUN chmod +x /workspaces/testdds/tools/launch.sh

# Default command (can be overridden in docker-compose)
CMD ["bash"]
