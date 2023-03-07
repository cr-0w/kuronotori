#include "injectDLL.h"
#include "injectShellcode.h"

char ok[] = ("(" KGRN "+" KRST ")");
char in[] = ("(" KYEL "*" KRST ")");
char er[] = ("(" KRED "!" KRST ")");

void bannerInfo() {
    std::cout << R"(				 
              __
          ___(__)___
         /          \
        |     ___    |
        |    ('v')   |
        |   ((___))  |
        |--/-"---"---|
)" << "\n";

    printf(KGRN "\tkuro no tori v1.0.1 (injector) \n" KRST);
    printf(KYEL "\tauthor: crow // @cr-0w [github]\n\n" KRST);

}

int main(void) {

    bannerInfo();
    DWORD choice;

    printf("\n%s please choose a process injection technique\n", in);
    printf("\n\t1) Shellcode Injection \n\t2) DLL Injection \n\n>>> ");

    scanf_s("%d", &choice);

    switch (choice) {
    case 1:
        printf("\n%s loading Shellcode Injection module...\n", in);
        injectShellcode();
        break;
    case 2:
        printf("\n%s loading DLL Injection module...\n", in);
        injectDLL();
        break;
    default:
        printf("\n%s invalid choice!\n", er);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
