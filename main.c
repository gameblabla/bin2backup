#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

// This can be changed to a lower address (like 0x7FC0), provided that you change your linker accordingly
#define RAM_TO_EXECUTE_CODE_FROM 0x008000

int main(int argc, char* argv[])
{
	uint32_t len = 0;
	uint32_t towrite_tmp;
	char* binary;
	int size = 0;
	int tsize = 0;
	FILE* fp;
	if (argc < 3)
	{
		printf("BIN2BACKUP\nTool for putting executable binaries in a format suitable for the backup memory");
		printf("\nNote that Mednafen only supports up to 64K for backup memory.\n");
		printf("\nAdditionally, code size is limited to the PC-FX's RAM.\n(you cannot execute code from ROM)\n");
		printf("\nbin2backup exe.bin final.sav\n");
		return 0;
	}
	
	/* First we copy our binary to memory */
	fp = fopen(argv[1], "rb");
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	binary = malloc(len);
	fread(binary, 1, len, fp);
	fclose(fp);
	
	/* Check if size exceeds the PC-FX's RAM */
	size = sizeof(header_pcfx) + len;
	if (size > 1048576 * 2)
	{
		printf("ERROR : Binary size exceeds PC-FX's RAM (2MB) !\nBecause it is not possible to execute code from ROM,\nit has to be copied to the console's RAM first.\nExiting...\n");
		free(binary);
		return 0;
	}
	
	/* Then we write our special header to the output file, our binary copied in memory and jump back to 0x8030 to change the header a bit */
	fp = fopen(argv[2], "wb");
	fwrite (header_pcfx , sizeof(char), sizeof(header_pcfx), fp);
	fwrite (binary , sizeof(char), len, fp);
	
	if (size < 65536)
	{
		tsize = (65536 - size);
		printf("Let's pad it to 64k for Mednafen\n");
	}
	else if (size > 1048576)
	{
		printf("WARNING : MEDNAFEN only supports up to 64k for backup memory.\nBinary size bigger than 1MB\nPadding it to 2MB but this may not work as\nthe PC-FX RAM is 2MB\n");
		tsize = ((1048576*2) - size);
	}
	else if (size > 65536)
	{
		printf("WARNING : MEDNAFEN only supports up to 64k for backup memory.\nPadding it to 1MB, given that the PC-FX's RAM is only 2MB.\n");
		tsize = (1048576 - size);
	}
	
	binary = malloc(tsize);
	memset(binary, 0, tsize);
	fwrite (binary , sizeof(char), tsize, fp);
	

	fseek(fp, 0x8030, SEEK_SET);
	towrite_tmp = 0x001000;
	fwrite (&towrite_tmp , sizeof(char), 4, fp);
	
	towrite_tmp = RAM_TO_EXECUTE_CODE_FROM;
	fwrite (&towrite_tmp , sizeof(char), 4, fp);
	
	towrite_tmp = len;
	fwrite (&towrite_tmp , sizeof(char), 4, fp);
	
	towrite_tmp = RAM_TO_EXECUTE_CODE_FROM;
	fwrite (&towrite_tmp , sizeof(char), 4, fp);
	
	fclose(fp);
	
	free(binary);
	return 0;
}
