#!/bin/bash

# Exit if fail
set -e

if [ "$TRAVIS_OS_NAME" = linux -o -z "$TRAVIS_OS_NAME" ]; then

    # Repository for clang
    sudo add-apt-repository -y ppa:h-rayflood/llvm
    sudo add-apt-repository -y ppa:h-rayflood/llvm-upper
    sudo add-apt-repository -y ppa:h-rayflood/gcc-upper

    # Update packages
    sudo apt-get update -qq

    # Install clang
    sudo apt-get -qq --allow-unauthenticated install clang-${CLANG_VERSION} clang++-${CLANG_VERSION}

    # Install libc++ for clang
    CWD=$(pwd)
    export CXXFLAGS="-std=c++0x -stdlib=libc++"
    svn co --quiet http://llvm.org/svn/llvm-project/libcxx/trunk libcxx
    cd libcxx/lib && bash buildit
    sudo cp ./libc++.so.1.0 /usr/lib/
    sudo mkdir /usr/include/c++/v1
    cd .. && sudo cp -r include/* /usr/include/c++/v1/
    cd /usr/lib && sudo ln -sf libc++.so.1.0 libc++.so
    sudo ln -sf libc++.so.1.0 libc++.so.1 && cd $CWD

    # Update alternatives for clang
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${CLANG_VERSION} 50
    sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${CLANG_VERSION} 50

elif [ "$TRAVIS_OS_NAME" = osx ]; then

    # Make sure xcode is setup
    xcode-select --install

    # Update brew
    brew update || echo "suppress failures in order to ignore warnings"
    brew doctor || echo "suppress failures in order to ignore warnings"
    brew list clang || echo "suppress failures in order to ignore warnings"
    sudo brew uninstall --force clang || "suppress failures in order to ignore warnings"
    brew search clang || echo "suppress failures in order to ignore warnings"

    # Install clang
    brew install clang || echo "suppress failures in order to ignore warnings"
    brew upgrade clang || echo "suppress failures in order to ignore warnings"
fi

# Export clang
export CXX="clang++-${CLANG_VERSION}"
export CC="clang-${CLANG_VERSION}"

# Debug information
clang --version
clang++ --version

