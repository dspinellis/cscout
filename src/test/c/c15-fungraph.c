#define macro() middlemacro()
#define middlemacro() innemacro()
#define innemacro() function1()
function1() {}
function2() {}
main() {
        macro();
        function2();
        function3();
        printf("Hello");
}
