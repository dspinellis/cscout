#define IS_SIMPLE_INT 12

#define IS_NESTED IS_SIMPLE_INT

#define IS_NOT_NESTED IS_NOT_VAR

#define IS_NOT_FLOAT 12.3

#define IS_EXPR (+12 * -0x3 + ~2 / (1 + (!5 - 4 % 7 << 8 >> 9 > 2 < 4 >= 5\
	<= 9 ==  2 != 3 & 8 ^ 9 | 10 && 11 || 12 ? 3 : 4 + ((3)))))

#define IS_EXPR_EXPANDED (1 + IS_SIMPLE_INT)

int a;
#define IS_NOT_VAR a

#define IS_UNUSED 0x1234
#define IS_NOT_UNBALANCED )
#define IS_NOT_KEYWORD for (;;)

void t(void) {
	int a1 = IS_SIMPLE_INT;
	double a2 = IS_NOT_FLOAT;
	int a3 = IS_EXPR;
	int a4 = IS_NOT_VAR;
	int a5 = IS_UNUSED;
	int a6 = (5 IS_NOT_UNBALANCED;
	int a7 = IS_NESTED;
	int a8 = IS_NOT_NESTED;
	int a9 = IS_EXPR_EXPANDED;

	IS_NOT_KEYWORD;
}
