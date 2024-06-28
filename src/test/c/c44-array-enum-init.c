enum asn1_tag {
	ASN1_BOOL	= 42,	/* Boolean */
};

enum token_type {
	DIRECTIVE_BOOLEAN,
	DIRECTIVE_INTEGER,
	NR__TOKENS
};

// Expected error; OK
int a[3] = {1, 2, 3, 4};

// Expected error; OK
int a[2] = {[2] = 42};

// No error
int a[3] = {1, 2, 3, };

// No error
int a[2] = {[1] = 42};

// No error
int a[2] = {[DIRECTIVE_BOOLEAN] = 42};

// No error
int a[NR__TOKENS] = {[DIRECTIVE_BOOLEAN] = ASN1_BOOL};

// No error
int a[NR__TOKENS] = {[NR__TOKENS - 1] = ASN1_BOOL};
