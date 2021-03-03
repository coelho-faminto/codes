#include "stdafx.h"
#include <Windows.h>
#include <sys/stat.h>
#include "load_assembly.h"
#include "detours/detours.h"

#pragma comment(lib, "detours/detours.lib")

typedef int
(WINAPI*
	_MessageBoxW)(
		_In_opt_ HWND hWnd,
		_In_opt_ LPCWSTR lpText,
		_In_opt_ LPCWSTR lpCaption,
		_In_ UINT uType);
_MessageBoxW __MessageBoxW = (_MessageBoxW)GetProcAddress(LoadLibrary(L"user32.dll"), "MessageBoxW");

typedef HMODULE
(WINAPI*
	_LoadLibraryExW)(
		_In_ LPCWSTR lpLibFileName,
		_Reserved_ HANDLE hFile,
		_In_ DWORD dwFlags
		);

typedef HMODULE
(WINAPI*
	_LoadLibraryExA)(
		_In_ LPCSTR lpLibFileName,
		_Reserved_ HANDLE hFile,
		_In_ DWORD dwFlags
		);

_LoadLibraryExW __LoadLibraryExW = (_LoadLibraryExW)GetProcAddress(LoadLibrary(L"kernel32.dll"), "LoadLibraryExW");
_LoadLibraryExA __LoadLibraryExA = (_LoadLibraryExA)GetProcAddress(LoadLibrary(L"kernel32.dll"), "LoadLibraryExA");

int
WINAPI
hkMessageBoxW(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType)
{
	wprintf(L"MessageBoxW: %s\n", lpText);
	return __MessageBoxW(hWnd, lpText, lpCaption, uType);
}

HMODULE
WINAPI
hkLoadLibraryExW(
	_In_ LPCWSTR lpLibFileName,
	_Reserved_ HANDLE hFile,
	_In_ DWORD dwFlags
)
{
	wprintf(L"LoadLibraryExW: %s\n", lpLibFileName);

	if (wcsstr(lpLibFileName, L"amsi.dll")) {
		wprintf(L"Detected amsi loading, will return NULL!\n");
		return (HMODULE)0;
	}

	return __LoadLibraryExW(lpLibFileName, hFile, dwFlags);
}

HMODULE
WINAPI
hkLoadLibraryExA(
	_In_ LPCSTR lpLibFileName,
	_Reserved_ HANDLE hFile,
	_In_ DWORD dwFlags
)
{
	printf("LoadLibraryExA: %s\n", lpLibFileName);

	if (strstr(lpLibFileName, "amsi.dll")) {
		printf("Detected amsi loading, will return NULL!\n");
		return (HMODULE)0;
	}

	return __LoadLibraryExA(lpLibFileName, hFile, dwFlags);
}

void detoursHook()
{
	DetourTransactionBegin();

	DetourAttach((PVOID*)&__MessageBoxW, (PVOID)hkMessageBoxW);
	DetourAttach((PVOID*)&__LoadLibraryExW, (PVOID)hkLoadLibraryExW);
	DetourAttach((PVOID*)&__LoadLibraryExA, (PVOID)hkLoadLibraryExA);

	DetourTransactionCommit();
}

int main(int argc, char* argv[])
{
	void* mem;
	struct stat fs;
	FILE* fd;

	detoursHook();

	stat("ExampleAssembly.exe", &fs);

	fd = fopen("ExampleAssembly.exe", "rb");
	if (fd == NULL) {
		printf("unable to open.\n");
		return 0;
	}

	mem = malloc(fs.st_size);

	if (mem == NULL) {
		printf("unable to allocate memory.\n");
		return 0;
	}

	fread(mem, 1, fs.st_size, fd);
	fclose(fd);

	load_assembly(mem, fs.st_size);

	free(mem);

	MessageBoxW(0, L"Message Box Test", L"My App", 0);

	getchar();
}