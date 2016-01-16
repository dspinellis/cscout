// workspace awk
#pragma echo "Processing workspace awk\n"
// project awk
#pragma echo "Processing project awk\n"
#pragma project "awk"
#pragma block_enter
#pragma echo "Entering directory awk\n"
#pragma pushd "awk"
// file awkgram.y
#pragma echo "Processing file awkgram.y\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#define yyclearin
#include ".cscout/cscout_incs.h"
#pragma process "awkgram.y"
#pragma process "ytab.h"

#pragma block_exit
#pragma echo "Done processing file awkgram.y\n"
// file b.c
#pragma echo "Processing file b.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "b.c"

#pragma block_exit
#pragma echo "Done processing file b.c\n"
// file lex.c
#pragma echo "Processing file lex.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "lex.c"

#pragma block_exit
#pragma echo "Done processing file lex.c\n"
// file lib.c
#pragma echo "Processing file lib.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "lib.c"

#pragma block_exit
#pragma echo "Done processing file lib.c\n"
// file main.c
#pragma echo "Processing file main.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "main.c"

#pragma block_exit
#pragma echo "Done processing file main.c\n"
// file maketab.c
#pragma echo "Processing file maketab.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "maketab.c"

#pragma block_exit
#pragma echo "Done processing file maketab.c\n"
// file parse.c
#pragma echo "Processing file parse.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "parse.c"

#pragma block_exit
#pragma echo "Done processing file parse.c\n"
// file proctab.c
#pragma echo "Processing file proctab.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "proctab.c"

#pragma block_exit
#pragma echo "Done processing file proctab.c\n"
// file run.c
#pragma echo "Processing file run.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "run.c"

#pragma block_exit
#pragma echo "Done processing file run.c\n"
// file tran.c
#pragma echo "Processing file tran.c\n"
#pragma block_enter
#pragma clear_defines
#pragma clear_pragma process
#include ".cscout/cscout_defs.h"
#include ".cscout/cscout_incs.h"
#pragma process "tran.c"

#pragma block_exit
#pragma echo "Done processing file tran.c\n"
#pragma echo "Exiting directory awk\n"
#pragma popd
#pragma block_exit
#pragma echo "Done processing project awk\n"
#pragma echo "Done processing workspace awk\n"
