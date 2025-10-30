#!/bin/bash
set -euo pipefail

GREEN="\e[1;32m"
RED="\e[1;31m"
YELLOW="\e[1;33m"
RESET="\e[0m"

VERBOSE_MAKE=""
VERBOSE_CMAKE=""
MAKE_ARGS=()
DDS_OPTION=""

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        fast)
            DDS_OPTION="-DUSE_FAST_DDS=ON -DUSE_CYCLONE_DDS=OFF"
            ;;
        cyclone)
            DDS_OPTION="-DUSE_FAST_DDS=OFF -DUSE_CYCLONE_DDS=ON"
            ;;
        --verbose)
            VERBOSE_MAKE="VERBOSE=1"
            VERBOSE_CMAKE="-DCMAKE_VERBOSE_MAKEFILE=ON"
            ;;
        *)
            MAKE_ARGS+=("$arg")
            ;;
    esac
done

# Ensure DDS option is set
if [[ -z "$DDS_OPTION" ]]; then
    echo -e "${RED}❌ Please specify DDS type: 'fast' or 'cyclone'.${RESET}"
    exit 1
fi

echo -e "${YELLOW}🛠 Starting build process...${RESET}"

cd "$(dirname "$0")/.." || exit 1
mkdir -p build
cd build || exit 1

echo -e "${YELLOW}📦 Running CMake...${RESET}"
if cmake .. ${DDS_OPTION} ${VERBOSE_CMAKE}; then
    echo -e "${GREEN}✅ CMake configuration successful.${RESET}"
else
    echo -e "${RED}❌ CMake failed. Please check the output above.${RESET}"
    exit 1
fi

echo -e "${YELLOW}⚙️  Building project...${RESET}"
if make ${VERBOSE_MAKE} "${MAKE_ARGS[@]:-}"; then
    echo -e "${GREEN}✅ Build completed successfully!${RESET}"
else
    echo -e "${RED}❌ Build failed. Please check the output above.${RESET}"
    exit 1
fi
