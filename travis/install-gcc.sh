#!/bin/bash

# Exit if fail
set -e

if [ "$TRAVIS_OS_NAME" = linux -o -z "$TRAVIS_OS_NAME" ]; then

    # Repository for gcc
    sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test

    # Update packages
    sudo apt-get update -qq

    # Install gcc
    sudo apt-get -qq --allow-unauthenticated install gcc-${LIBSTDC_VERSION} g++-${LIBSTDC_VERSION}

    # Update alternatives for gcc
    sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${LIBSTDC_VERSION} 50
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${LIBSTDC_VERSION} 50

elif [ "$TRAVIS_OS_NAME" = osx ]; then

    # Make sure xcode is setup
    xcode-select --install

    # Update brew
    brew update || echo "suppress failures in order to ignore warnings"
    brew doctor || echo "suppress failures in order to ignore warnings"
    brew list gcc || echo "suppress failures in order to ignore warnings"
    sudo brew uninstall --force gcc || "suppress failures in order to ignore warnings"
    brew search gcc || echo "suppress failures in order to ignore warnings"

    # Install gcc
    brew install homebrew/versions/gcc${LIBSTDC_VERSION} || echo "suppress failures in order to ignore warnings"
    brew upgrade homebrew/versions/gcc${LIBSTDC_VERSION} || echo "suppress failures in order to ignore warnings"

fi

# Export gcc
export CXX="g++-${LIBSTDC_VERSION}"
export CC="gcc-${LIBSTDC_VERSION}"

# Debug information
gcc --version
g++ --version

