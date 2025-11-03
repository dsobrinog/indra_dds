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
        curl \
        libasio-dev \
        libtinyxml2-dev \
        libssl-dev \
        python3 \
        python3-pip \
        openjdk-11-jdk  \
        flex \
        bison && \
    rm -rf /var/lib/apt/lists/*
    
    
# ==============================
# Container set up for docker compose
# ==============================
# Set working directory
WORKDIR /workspaces/fastdds
    
# Copy project files into the container
# COPY . .

# Build FastDDS application
# RUN mkdir -p build && \
#     cd build && \
#     cmake .. && \
#     make -j$(nproc)

# Ensure launch.sh is executable
# RUN chmod +x /workspaces/fastdds/tools/launch.sh

# Default command (can be overridden in docker-compose)
CMD ["bash"]
