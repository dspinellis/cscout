// To test this you need to set the debug point in parse.y:constant_expression

int u;

int ar01[45];
int ar02[u + 12];
int ar03[12 + u];
int ar04[u + u];
int ar05[12 + 12];
int ar08[0 || 0];
int ar07[0 || 1];
int ar06[1 || 0];
int ar09[1 || 1];
int ar10[0 && 0];
int ar11[0 && 1];
int ar12[1 && 0];
int ar13[1 && 1];
int ar14[8 | 4];
int ar15[0xa5 ^ 0xff];
int ar16[0xa5 & 0xf0];
int ar17[1 == 2];
int ar18[1 == 1];
int ar19[1 != 2];
int ar20[1 != 1];
int ar21[1 > 5];
int ar22[5 > 5];
int ar23[1 < 5];
int ar24[5 < 1];
int ar25[8 >> 1];
int ar26[8 << 1];
int ar27[5 - 2];
int ar28[5 * 2];
int ar29[5 / 2];
int ar30[5 % 2];
int ar31[-5 + 20];
int ar32[!!5];
int ar33[~5];


