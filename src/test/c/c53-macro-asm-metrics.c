#define  example_function(input, label) { \
    int output = 0; \
 \
    __asm__ goto ( \
        "cmp %[in], 0\n"         \
        "jne %l[label]\n"        \
        : [out] "=r"(output)     \
        : [in] "r"(input)        \
        :                        \
        : label                  \
    ); \
 \
    printf("No jump occurred. Output: %d\n", output); \
    return; \
 \
label: \
    output = 42;\
    printf("Jumped to label! Output: %d\n", output); \
}

int main() {
    printf("Calling with input = 0:\n");
    example_function(0, l1);        // Does not jump
    printf("\nCalling with input = 1:\n");
    example_function(1, l2);        // Jumps
    return 0;
}
