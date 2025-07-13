#!/bin/bash

set -e

echo "Installing system dependencies for PieEngine..."

# Update package list
sudo apt-get update

# Install essential build tools
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    git \
    curl \
    wget \
    unzip \
    tar

# Install Mono (required for vcpkg)
echo "Installing Mono..."
sudo apt-get install -y \
    mono-complete \
    mono-devel \
    mono-runtime

# Install additional development libraries
sudo apt-get install -y \
    libssl-dev \
    libcurl4-openssl-dev \
    libxml2-dev \
    libpng-dev \
    libjpeg-dev \
    libfreetype6-dev \
    libfontconfig1-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libxext-dev \
    libxrender-dev \
    libxss-dev \
    libxfixes-dev \
    libx11-dev \
    libx11-xcb-dev \
    libxcb1-dev \
    libxcb-render0-dev \
    libxcb-shape0-dev \
    libxcb-xfixes0-dev \
    libxcb-keysyms1-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-shm0-dev \
    libxcb-randr0-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libxcb-util-dev

# Install Python (required for some vcpkg ports)
sudo apt-get install -y \
    python3 \
    python3-pip \
    python3-dev

# Install additional tools
sudo apt-get install -y \
    doxygen \
    graphviz \
    valgrind \
    gdb \
    lldb

echo "System dependencies installed successfully!"

# Verify Mono installation
echo "Verifying Mono installation..."
mono --version

echo "All dependencies installed and verified!" 