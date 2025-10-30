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
    # Install Cyclone DDS from source
    # ==============================
    RUN apt-get update && \
    apt-get install -y ca-certificates curl gnupg && \
    update-ca-certificates && \
    git config --global http.sslVerify false
    
    # ------------------------------
    # Set up work directories
    # ------------------------------
    WORKDIR /opt

    # ------------------------------
    # Build Cyclone DDS
    # ------------------------------
    RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds.git cyclonedds-src && \
    cd cyclonedds-src && git checkout tags/0.10.1 && \
    mkdir build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/opt/cyclonedds \
        -DBUILD_SHARED_LIBS=ON \
        -DBUILD_IDLC=ON && \
    cmake --build . --target install -j$(nproc)

    # ------------------------------
    # Build Cyclone DDS C++ API
    # ------------------------------
    RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds-cxx.git cyclonedds-cxx-src && \
        cd cyclonedds-cxx-src && git checkout tags/0.10.1 && \
        mkdir build && cd build && \
        cmake .. \
            -DCMAKE_PREFIX_PATH=/opt/cyclonedds \
            -DCMAKE_INSTALL_PREFIX=/opt/cyclonedds \
            -DCMAKE_BUILD_TYPE=Release \
            -DENABLE_LEGACY=NO \
            -DBUILD_SHARED_LIBS=ON && \
        cmake --build . --target install -j$(nproc)

    # ------------------------------
    # Set environment variables
    # ------------------------------
    ENV CYCLONEDDS_HOME=/opt/cyclonedds
    ENV CYCLONEDDS_CXX_HOME=/opt/cyclonedds-cxx
    ENV PATH=$CYCLONEDDS_HOME/bin:$CYCLONEDDS_CXX_HOME/bin:$PATH
    ENV LD_LIBRARY_PATH=$CYCLONEDDS_HOME/lib:$CYCLONEDDS_CXX_HOME/lib:$LD_LIBRARY_PATH
    ENV CMAKE_PREFIX_PATH=$CYCLONEDDS_HOME

    # Update linker cache
    RUN echo "/opt/cyclonedds/lib" >> /etc/ld.so.conf.d/cyclonedds.conf && \
        echo "/opt/cyclonedds-cxx/lib" >> /etc/ld.so.conf.d/cyclonedds.conf && \
    ldconfig
    
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
