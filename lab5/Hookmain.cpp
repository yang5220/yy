#include <windows.h>

LONG IATHook(
	__in_opt void* pImageBase,
	__in_opt const char* pszImportDllName,
	__in const char* pszRoutineName,
	__in void* pFakeRoutine,
	__out HANDLE* phHook
);

LONG UnIATHook(__in HANDLE hHook);

void* GetIATHookOrign(__in HANDLE hHook);

typedef bool(__stdcall *LPFN_WriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);


HANDLE g_hHook_WriteFile = NULL;

//////////////////////////////////////////////////////////////////////////


bool __stdcall Fake_WriteFile(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,   //要写入的长度
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped) {
	char buffer[] = "you have been hacked!\n";
	DWORD BytesToWrite = (DWORD)strlen(buffer);

	LPFN_WriteFile fnOrigin = (LPFN_WriteFile)GetIATHookOrign(g_hHook_WriteFile);

	return fnOrigin(hFile, buffer, BytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

BOOL WINAPI DllMain(HINSTANCE hinstDll,DWORD dwReason,LPVOID lpvRevered)
{

	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		IATHook(
			GetModuleHandleW(NULL),
			"kernel32.dll",
			"WriteFile",
			Fake_WriteFile,
			&g_hHook_WriteFile
		);
		break;
	case DLL_PROCESS_DETACH:
		UnIATHook(g_hHook_WriteFile);
		break;
	}
	return TRUE;

}