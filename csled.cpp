#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <tchar.h>
#include<string>
#include<stdlib.h>
#include"SerialPort.h"

using namespace std;

DWORD dwGetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
	DWORD dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	MODULEENTRY32 ModuleEntry32 = { 0 };
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &ModuleEntry32))
	{
		do {
			if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
			{
				dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &ModuleEntry32));
	}
	CloseHandle(hSnapshot);
	return dwModuleBaseAddress;
}

const char* portName = "\\\\.\\COM3";
SerialPort* arduino;
#define DATA_LENGTH 255
char receivedString[DATA_LENGTH];

int main() {

	arduino = new SerialPort(portName);

	DWORD procID;
	HWND gameWindow;
	char moduleClient[] = "client.dll";
	char moduleServer[] = "server.dll";

	DWORD off1;
	DWORD isC4on = 0;
	DWORD c4Time = 0;

	const char* sendString;
	int hasRead;
	bool hasWritten;
	int ledTime;

	gameWindow = FindWindow(NULL, "Counter-Strike: Global Offensive");

	if (gameWindow == NULL) {
		MessageBox(NULL, "Error cannot find window!", "Error!", MB_OK + MB_ICONERROR);
		Sleep(1000);
		exit(-1);
	}
	else {
		GetWindowThreadProcessId(gameWindow, &procID);
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
		DWORD baddressC4 = dwGetModuleBaseAddress(_T(moduleServer), procID);
		DWORD baddressTime = dwGetModuleBaseAddress(_T(moduleServer), procID);

		if (!procID) {
			MessageBox(NULL, "Cannot open process!", "Error!", MB_OK + MB_ICONERROR);
			Sleep(1000);
			exit(-1);
		}
		else {
			while (arduino->isConnected()) {

				ReadProcessMemory(handle, (LPCVOID)(baddressC4 + 0x00B48C70), &c4Time, sizeof(isC4on), NULL);
				//ReadProcessMemory(handle, (LPCVOID)(baddressTime + 0x4A61260), &isC4on, sizeof(c4Time), NULL);
				ReadProcessMemory(handle, (LPCVOID)(c4Time + 0x1D4C), &off1, sizeof(off1), NULL);

				isC4on = off1;

				if (isC4on == 1) {
					sendString = "1";
					bool hasWritten = arduino->writeSerialPort(sendString, DATA_LENGTH);

					if (hasWritten) {
						int hasRead = arduino->readSerialPort(receivedString, DATA_LENGTH);
						if (hasRead) {
							std::cout << "ON: " << receivedString << std::endl;
						}
						else std::cerr << "[1] Error occured reading data" << std::endl;
					}
					else std::cerr << "[1] Data was not written" << std::endl;
				}

				else if (isC4on == 0) {
					sendString = "0";
					bool hasWritten = arduino->writeSerialPort(sendString, DATA_LENGTH);

					if (hasWritten) {
						int hasRead = arduino->readSerialPort(receivedString, DATA_LENGTH);

						if (hasRead) {
							std::cout << "OFF: " << receivedString << std::endl;
						}
						else std::cerr << "[0] Error occured reading data" << std::endl;
					}
					else std::cerr << "[0] Data was not written" << std::endl;
				}

			}
		}
	}
	cin.get();
}