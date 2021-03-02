#include "stdafx.h"
#include <Windows.h>
#include <sys/stat.h>
#include "load_assembly.h"

int main(int argc, char* argv[])
{
	void* mem;
	struct stat fs;
	FILE* fd;

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
}