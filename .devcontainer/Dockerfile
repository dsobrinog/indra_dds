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
        wget \
        libasio-dev \
        libtinyxml2-dev \
        openjdk-11-jdk

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
