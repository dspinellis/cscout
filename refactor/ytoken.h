/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Token definitions for btyacc
 * Kept here to keep them in sync between the preprocessor expression
 * and the main grammar.
 *
 * $Id: ytoken.h,v 1.4 2002/09/07 09:47:15 dds Exp $
 */


/* Define terminal tokens */

/* keywords */
%token AUTO            DOUBLE          INT             STRUCT
%token BREAK           ELSE            LONG            SWITCH
%token CASE            ENUM            REGISTER        TYPEDEF
%token CHAR            EXTERN          RETURN          UNION
%token TCONST          FLOAT           SHORT           UNSIGNED
%token CONTINUE        FOR             SIGNED          TVOID
%token DEFAULT         GOTO            SIZEOF          VOLATILE
%token DO              IF              STATIC          WHILE

/* ANSI Grammar suggestions */
%token IDENTIFIER         STRING_LITERAL
%token FLOAT_CONST        INT_CONST        


/* 
 * New Lexical element, whereas ANSI suggested non-terminal
 * Lexer will tell part of the difference between this and
 * an  identifier!   An  identifier  that is CURRENTLY in scope as a
 * typedef name is provided to the parser as a TYPEDEF_NAME.
 */
%token TYPEDEF_NAME 

/* Multi-Character operators */
%token  PTR_OP                       /*    ->                              */
%token  INC_OP DEC_OP                /*    ++      --                      */
%token  LEFT_OP RIGHT_OP             /*    <<      >>                      */
%token  LE_OP GE_OP EQ_OP NE_OP      /*    <=      >=      ==      !=      */
%token  AND_OP OR_OP                 /*    &&      ||                      */
%token  ELLIPSIS                     /*    ...                             */

/* modifying assignment operators */
%token MUL_ASSIGN  DIV_ASSIGN    MOD_ASSIGN   /*   *=      /=      %=      */
%token ADD_ASSIGN  SUB_ASSIGN                 /*   +=      -=              */
%token LEFT_ASSIGN RIGHT_ASSIGN               /*   <<=     >>=             */
%token AND_ASSIGN  XOR_ASSIGN    OR_ASSIGN    /*   &=      ^=      |=      */


/* Tokens needed by the pre-processor */
%token CPP_CONCAT SPACE ABSFNAME PATHFNAME CHAR_LITERAL PP_NUMBER

/* gcc __asm__ directive */
%token GNUC_ASM
/* gcc __typeof extension */
%token TYPEOF
/* MSC _asm directive */
%token MSC_ASM
