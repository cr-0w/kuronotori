#include "injectDLL.h"

PVOID rBufferDLL;
HANDLE hProcessDLL;
DWORD pLevelDLL = 1;
DWORD processCounterDLL = 0;
PWTS_PROCESS_INFO_EX procInfoDLL = NULL; // WTS_PROCESS_INFO_EX structure (has more flags :3) 

// CHANGE THIS
wchar_t dllLocation[MAX_PATH] = L"C:\\Users\\Niko Bellic\\Desktop\\example.dll";

int injectDLL(void) {

    printf("%s loaded DLL Injection module!\n", ok);

    if (!WTSEnumerateProcessesEx(
        WTS_CURRENT_SERVER,
        &pLevelDLL,
        WTS_ANY_SESSION,
        (LPTSTR*)&procInfoDLL,
        &processCounterDLL
    )) {

        printf("\n%s something went wrong. you're on your own.", er);
        return EXIT_FAILURE;

    }

    printf("%s press any key to continue", in);
    fflush(stdin); // clear input buffer 
    _getch();
    system("cls");
    printf("%s starting process enumeration...\n", in);

    for (DWORD iterator = 1; iterator <= processCounterDLL; iterator++) {

        printf("\n%s %S [%d]\n", in, procInfoDLL->pProcessName, iterator);
        printf("\n\t%s PID (%d)", ok, procInfoDLL->ProcessId);
        printf("\n\t%s session ID (%d)", ok, procInfoDLL->SessionId);
        printf("\n\t%s process name = %S", ok, procInfoDLL->pProcessName);
        printf("\n\t%s process handle count = %d", ok, procInfoDLL->HandleCount);
        printf("\n\t%s number of threads = %d\n", ok, procInfoDLL->NumberOfThreads);
        procInfoDLL++;
    }

    printf("\n%s processes found: %d", ok, processCounterDLL);

    DWORD PID;
    printf("\n%s select a PID to inject into\n\n>>> ", in);
    std::cin >> PID;
    system("cls");
    printf("\n%s PID selected (%d)", in, PID);

    hProcessDLL = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        PID);

    if (!hProcessDLL || hProcessDLL == NULL) {
        printf("%s could not create handle to process [%d]", er, PID);
        return EXIT_FAILURE;
    }

    printf("\n%s attached/created handle to process (%d)\n", ok, PID);

    WCHAR procPath[MAX_PATH];
    DWORD procSize = MAX_PATH;

    if (QueryFullProcessImageNameW(
        hProcessDLL,
        0, // DON'T SET THIS TO PROCESS_NAME_NATIVE (0x00000001)
        procPath,
        &procSize)
        ) {

        printf("\n\t%s process path = %ls", in, procPath);

        WCHAR* exeName = wcsrchr(procPath, L'\\');
        if (exeName == NULL) {
            printf("\n\t%s unable to extract name from path", er);
            exeName = procPath;
        }

        else {
            exeName++;
        }

        printf("\n\t%s process name = %ls", in, exeName);
    }

    else {
        printf("\n%s something went wrong. you're on your own. good luck.", er);
        exit(1);
    }

    DWORD arch;
    if (!GetBinaryTypeW(procPath, &arch)) {
        printf("\n\t%s failed to get binary arch.", er);
    }

    switch (arch) {

    case SCS_32BIT_BINARY:
        printf("\n\t%s process architecture = 32-bit", in);
        break;

    case SCS_64BIT_BINARY:
        printf("\n\t%s process architecture = 64-bit", in);
        break;

    case SCS_WOW_BINARY:
        printf("\n\t%s process architecture = 16-bit", in);
        break;

    default:
        printf("\n\t%s process architecture = UNKNOWN.", in);
        break;
    }

    DWORD PROCID = GetProcessId(hProcessDLL);
    printf("\n\t%s process_id = %i", in, PROCID);

    DWORD PROCVERSION = GetProcessVersion(PROCID);
    printf("\n\t%s process_version = %lu\n\n", in, PROCVERSION);

    printf("%s path to DLL set", ok);
    printf("\n\n\t%s dllLocation[] ---> ['%S']\n\n", in, dllLocation);
    printf("%s allocating memory within process...\n", in);

    rBufferDLL = VirtualAllocEx(hProcessDLL,
        rBufferDLL,
        sizeof(dllLocation),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);

    printf("%s allocated space within process\n\n", ok);
    printf("\t%s VirtualAllocEx() [flProtect ---> PAGE_READWRITE (RW)]\n", in);
    printf("\t%s VirtualAllocEx() [flAllocationType ---> MEM_COMMIT | MEM_RESERVE]\n\n", in);
    printf("%s writing to process memory...\n", in);

    WriteProcessMemory(hProcessDLL,
        rBufferDLL,
        (LPVOID)dllLocation,
        sizeof(dllLocation),
        NULL);

    printf("%s wrote to process memory\n\n", ok);
    printf("\t%s getting handle on KERNEL32 [KERNEL32 -> LoadLibraryW()]\n", in);
    printf("\t%s starting PTHREAD_START_ROUTINE...\n\n", in);

    PTHREAD_START_ROUTINE startRoutine = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");

    printf("%s PTHREAD_START_ROUTINE done\n", ok);
    printf("%s got handle on KERNEL32 module\n", ok);
    printf("%s got address of LoadLibraryW()\n", ok);
    printf("\n\t%s LoadLibraryW() @ 0x%p\n\n", in, (void*)startRoutine);

    printf("%s creating remote thread...\n", in);

    CreateRemoteThread(hProcessDLL,
        NULL,
        0,
        startRoutine,
        rBufferDLL,
        0,
        NULL);

    printf("%s thread created, enjoy!\n", ok);
    printf("%s closing handle to process...\n", in);

    CloseHandle(hProcessDLL);

    printf("%s finished", ok);

    return EXIT_SUCCESS;
}