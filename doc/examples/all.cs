#pragma echo "Processing workspace FreeBSD kernel\n"

#pragma echo "Entering directory sys/i386/compile/LINT\n"
#pragma pushd "sys/i386/compile/LINT"
#pragma echo "Processing project i386\n"
#pragma project "i386"
#pragma block_enter
#include "kernel.cs"
#pragma echo "Exiting directory sys/i386/compile/LINT\n"
#pragma popd
#pragma echo "Done processing project i386\n"
#pragma block_exit

// Known good
#pragma echo "Entering directory sys/amd64/compile/GENERIC\n"
#pragma pushd "sys/amd64/compile/GENERIC"
#pragma echo "Processing project amd64\n"
#pragma project "amd64"
#pragma block_enter
#include "kernel.cs"
#pragma echo "Exiting directory sys/amd64/compile/GENERIC\n"
#pragma popd
#pragma echo "Done processing project amd64\n"
#pragma block_exit

// Known good
#pragma echo "Entering directory sys/alpha/compile/LINT\n"
#pragma pushd "sys/alpha/compile/LINT"
#pragma echo "Processing project alpha\n"
#pragma project "alpha"
#pragma block_enter
#include "kernel.cs"
#pragma echo "Exiting directory sys/alpha/compile/LINT\n"
#pragma popd
#pragma echo "Done processing project alpha\n"
#pragma block_exit

// Known good
#pragma echo "Entering directory sys/ia64/compile/GENERIC\n"
#pragma pushd "sys/ia64/compile/GENERIC"
#pragma echo "Processing project ia64\n"
#pragma project "ia64"
#pragma block_enter
#include "kernel.cs"
#pragma echo "Exiting directory sys/ia64/compile/GENERIC\n"
#pragma popd
#pragma echo "Done processing project ia64\n"
#pragma block_exit

// Known good
#pragma echo "Entering directory sys/sparc64/compile/LINT\n"
#pragma pushd "sys/sparc64/compile/LINT"
#pragma echo "Processing project sparc64\n"
#pragma project "sparc64"
#pragma block_enter
#include "kernel.cs"
#pragma echo "Exiting directory sys/sparc64/compile/LINT\n"
#pragma popd
#pragma echo "Done processing project sparc64\n"
#pragma block_exit

