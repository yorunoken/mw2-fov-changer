#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

DWORD GetProcessIdOfProgram(const char* processName) {
	HANDLE hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hProc == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	DWORD processId = 0;

	if (Process32First(hProc, &processEntry)) {
		do {
			if (!strcmp(processName, processEntry.szExeFile)) {
				processId = processEntry.th32ProcessID;
				break;
			}
		} while (Process32Next(hProc, &processEntry));
	}

	CloseHandle(hProc);
	return processId;
}

int main(int argc, char* argv[]) {
	const char* processName = "iw4mp.exe";
	DWORD processId = GetProcessIdOfProgram(processName);
	if (processId == 0) {
		std::cout << "Process not found!" << std::endl;
		return 1;
	}
	std::cout << "Process ID of " << processName << ": " << processId << std::endl;

	std::cout << "Opening process." << std::endl;
	HANDLE hProc = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, processId);
	if (hProc == NULL) {
		std::cout << "Failed to open process. Error: " << GetLastError() << std::endl;
		return 1;
	}

	std::cout << "Reading memory." << std::endl;

	uintptr_t address = 0x0073CE6B; // Will try to figure out the adress first

	int buffer = 0;
	SIZE_T bytesRead;
	if (ReadProcessMemory(hProc, (LPCVOID)address, &buffer, sizeof(buffer), &bytesRead)) {
		std::cout << "Value at adress 0x" << std::hex << address << ":" << buffer << std::endl;
	}
	else {
		std::cout << "Failed to read memory at address 0x" << std::hex << address << ". Error: " << GetLastError() << std::endl;
	}

	std::cout << "Quitting." << std::endl;
	CloseHandle(hProc);

	return 0;
}
