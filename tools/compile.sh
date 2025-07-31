#!/bin/bash

set -euo pipefail

GREEN="\e[1;32m"
RED="\e[1;31m"
YELLOW="\e[1;33m"
RESET="\e[0m"

VERBOSE_MAKE=""
VERBOSE_CMAKE=""
MAKE_ARGS=()

# Parse arguments
for arg in "$@"; do
    if [[ "$arg" == "--verbose" ]]; then
        VERBOSE_MAKE="VERBOSE=1"
        VERBOSE_CMAKE="-DCMAKE_VERBOSE_MAKEFILE=ON"
    else
        MAKE_ARGS+=("$arg")
    fi
done

echo -e "${YELLOW}🛠 Starting build process...${RESET}"

cd "$(dirname "$0")/.." || exit 1
mkdir -p build
cd build || exit 1

echo -e "${YELLOW}📦 Running CMake...${RESET}"
if cmake .. ${VERBOSE_CMAKE}; then
    echo -e "${GREEN} CMake configuration successful.${RESET}"
else
    echo -e "${RED}❌ CMake failed. Please check the output above.${RESET}"
    exit 1
fi

echo -e "${YELLOW}⚙️  Building project...${RESET}"
if make ${VERBOSE_MAKE} "${MAKE_ARGS[@]:-}"; then
    echo -e "${GREEN} Build completed successfully!${RESET}"
else
    echo -e "${RED}❌ Build failed. Please check the output above.${RESET}"
    exit 1
fi
