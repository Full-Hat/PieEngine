FROM --platform=linux/arm64 ubuntu:latest

RUN apt-get update && apt-get install -y cmake build-essential ninja-build git curl zip unzip tar

RUN apt-get update && apt-get install -y wget gnupg software-properties-common lsb-release
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && add-apt-repository "deb http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-20 main" \
    && apt-get update
RUN apt-get install -y clang-20 lld-20 llvm-20-tools llvm-20-dev

# GLFW requirements for X11
RUN apt-get install -y libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev libwayland-dev pkg-config

# MacOS and Ubuntu dependencies
RUN apt-get update && apt-get install -y autoconf automake autoconf-archive
RUN apt-get update && apt-get install -y libtool python3-jinja2

# Install Xvfb for virtual display server (essential for GLFW tests in headless environments)
RUN apt-get install -y xvfb x11-utils

RUN rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app
RUN [ ! -d "./build" ] || rm ./build -r
RUN git submodule update --init --recursive

WORKDIR /app/vcpkg
RUN ./bootstrap-vcpkg.sh
RUN ./vcpkg install

WORKDIR /app
RUN cmake -DCMAKE_C_COMPILER=/usr/bin/clang-20 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-20 --preset VcpkgDebug
RUN cmake --build build/debug

# Create a script to run tests with virtual display
RUN echo '#!/bin/bash\n\
# Start Xvfb virtual display server\n\
Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &\n\
export DISPLAY=:99\n\
\n\
# Wait a moment for Xvfb to start\n\
sleep 2\n\
\n\
# Run the tests\n\
ctest --output-on-failure --extra-verbose\n\
\n\
# Clean up\n\
pkill Xvfb' > /app/run_tests.sh && chmod +x /app/run_tests.sh

WORKDIR /app/build/debug

# Run tests with virtual display
CMD ["/app/run_tests.sh"]
