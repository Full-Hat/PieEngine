FROM --platform=linux/arm64 ubuntu:latest

WORKDIR /app
COPY . /app
RUN [ ! -d "./build" ] || rm ./build -r
RUN ./installation_scripts/install_ubuntu_arm64.sh install_system
RUN git submodule update --init --recursive

RUN ./installation_scripts/install_ubuntu_arm64.sh install_vcpkg

WORKDIR /app
RUN ./installation_scripts/install_ubuntu_arm64.sh build

# Create a script to run tests with virtual display
RUN ./installation_scripts/install_ubuntu_arm64.sh test_all
