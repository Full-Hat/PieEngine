#!/usr/bin/env bash
set -e

# =========================
#  CONFIGURATION
# =========================
STAGES=("install_system" "install_vcpkg" "build" "test_all")

# =========================
#  UTILITIES
# =========================
print_help() {
    echo "Usage: $0 [STAGE1] [STAGE2] [STAGE3]"
    echo
    echo "Runs installation and build stages for the C++ project."
    echo
    echo "Stages:"
    echo "  install_system   Install required system dependencies."
    echo "  install_vcpkg    Install or bootstrap vcpkg package manager."
    echo "  build            Configure and build the project."
    echo "  test_all         Run all tests."
    echo
    echo "Examples:"
    echo "  $0                 # Run all stages (system, vcpkg, build)"
    echo "  $0 install_system  # Only run system packages installation"
    echo "  $0 install_vcpkg   # Only run vcpkg setup"
    echo "  $0 build           # Only build the project"
    echo "  $0 test_all       # Only run tests"
    echo
    echo "Options:"
    echo "  -h, --help         Show this help message and exit"
}

run_cmd() {
    echo "→ $*"
    "$@"
}

# =========================
#  STAGES IMPLEMENTATION
# =========================

install_system() {
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  brew install llvm@20 cmake ninja vcpkg
}

install_vcpkg() {
  ./vcpkg/bootstrap-vcpkg.sh
  ./vcpkg/vcpkg install --binarycaching
}

build() {
  export SDKROOT=$(xcrun --show-sdk-path)
  cmake -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm@20/bin/clang\
        -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm@20/bin/clang++\
        -DCMAKE_OSX_SYSROOT="$SDKROOT"\
        --preset VcpkgDebug
  cmake --build build/debug
}

test_all() {
  ctest --test-dir ./build/debug --output-on-failure --extra-verbose
}

# =========================
#  MAIN LOGIC
# =========================

if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    print_help
    exit 0
fi

# If no args: run all stages
if [ $# -eq 0 ]; then
    for stage in "${STAGES[@]}"; do
        "$stage"
    done
    exit 0
fi

# Validate and execute selected stages
selected_stages=()
for arg in "$@"; do
    if [[ " ${STAGES[*]} " == *" $arg "* ]]; then
        selected_stages+=("$arg")
    else
        echo "Unknown stage: $arg"
        echo "Use --help for usage information."
        exit 1
    fi
done

# If all stages specified manually, print hint
if [ ${#selected_stages[@]} -eq ${#STAGES[@]} ]; then
    echo "Note: You provided all stages. You could simply run '$0' with no arguments."
fi

# Run selected stages in order
for stage in "${selected_stages[@]}"; do
    "$stage"
done
