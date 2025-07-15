FROM ubuntu:latest

RUN apt-get update && apt-get install -y cmake build-essential ninja-build git curl zip unzip tar

RUN apt-get update && apt-get install -y wget gnupg software-properties-common lsb-release
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && add-apt-repository "deb http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-20 main" \
    && apt-get update
RUN apt-get install -y clang-20 lld-20 llvm-20-tools llvm-20-dev

    # GLFW requirements
RUN apt-get install -y libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev pkg-config

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

CMD ["./build/myapp"]
