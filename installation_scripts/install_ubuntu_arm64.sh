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

is_root() {
    [ "$(id -u)" -eq 0 ]
}

# Sudo handling
if is_root; then
    SUDO=""
else
    SUDO=$(command -v sudo || true)
fi

run_cmd() {
    echo "→ $*"
    "$@"
}

has_display() {
    # Detect if a display environment is available
    if [ -n "$DISPLAY" ] && [ -n "$(command -v xdpyinfo 2>/dev/null)" ] && xdpyinfo >/dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# =========================
#  STAGES IMPLEMENTATION
# =========================

install_system() {
  echo "=== [Stage 1/4] Installing system dependencies ==="
  # common stuff
  run_cmd $SUDO apt-get update && $SUDO apt-get install -y cmake build-essential ninja-build git curl zip unzip tar

  # clang
  run_cmd $SUDO apt-get update && $SUDO apt-get install -y wget gnupg software-properties-common lsb-release

  run_cmd wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | $SUDO apt-key add - \
              && $SUDO add-apt-repository "deb http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-20 main" \
              && $SUDO apt-get update

  run_cmd $SUDO apt-get install -y clang-20 lld-20 llvm-20-tools llvm-20-dev

  # glfw dependencies
  run_cmd $SUDO apt-get install -y libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev libwayland-dev pkg-config

  # Ubuntu/arm64 dependencies
  run_cmd $SUDO apt-get update && $SUDO apt-get install -y autoconf automake autoconf-archive
  run_cmd $SUDO apt-get update && $SUDO apt-get install -y libtool python3-jinja2

  if ! has_display; then
    # BEGIN FOR HEADLESS TESTS, DON'T USE ON SYSTEMS WITH A DISPLAY
    # Install Xvfb for virtual display server (essential for GLFW tests in headless environments)
    run_cmd $SUDO apt-get install -y xvfb x11-utils
    # END FOR HEADLESS TESTS, DON'T USE ON SYSTEMS WITH A DISPLAY
  fi

  run_cmd $SUDO rm -rf /var/lib/apt/lists/*
}

install_vcpkg() {
  echo "=== [Stage 2/4] Installing vcpkg packages ==="
  # Install package manager & package dependencies
  run_cmd ./vcpkg/bootstrap-vcpkg.sh
  run_cmd ./vcpkg/vcpkg install
}

build() {
  echo "=== [Stage 3/4] Building the project ==="
  # Build project
  run_cmd cmake -DCMAKE_C_COMPILER=/usr/bin/clang-20 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-20 --preset VcpkgDebug
  run_cmd cmake --build build/debug
}

test_all() {
  echo "=== [Stage 4/4] Testing the project ==="

  if ! has_display; then
    # BEGIN FOR HEADLESS TESTS, DON'T USE ON SYSTEMS WITH A DISPLAY
    # Start Xvfb virtual display server
    run_cmd Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &
    run_cmd export XVFB_PID=$!
    run_cmd export DISPLAY=:99

    # Wait a moment for Xvfb to start
    run_cmd sleep 2
    # END FOR HEADLESS TESTS, DON'T USE ON SYSTEMS WITH A DISPLAY
  fi

  # Run the tests
  run_cmd ctest --test-dir ./build/debug --output-on-failure --extra-verbose

  # Cleanup
  if [ -n "$XVFB_PID" ]; then
    echo "Stopping Xvfb..."
    kill $XVFB_PID || true
    wait $XVFB_PID 2>/dev/null || true
    unset XVFB_PID
  fi
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
