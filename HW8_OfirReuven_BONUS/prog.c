/*********************************
* Class: MAGSHIMIM C2			 *
* Week:                			 *
* Name:                          *
* Credits:                       *
**********************************/
#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#define REQUIRED_ARGS 4
#define FILE_INDEX 1
#define START_POINT_INDEX 2
#define END_POINT_INDEX 3
int createCopy(FILE* infectedFile, int startPoint, int endPoint, char* copyName);
int main(int argc, char** argv)
{
	FILE* infectedFile = NULL;
	int result = 0;

	if (argc != REQUIRED_ARGS)
	{
		printf("Invalid args. usage: <program> <file> <startByte> <endByte>");
		return -1;
	}

	infectedFile = fopen(argv[FILE_INDEX], "rb");
	
	// Creates copy
	result = createCopy(infectedFile, atoi(argv[START_POINT_INDEX]), atoi(argv[END_POINT_INDEX]), "VirusSign");
	
	if (!result)
	{
		printf("Error copying virus sign");
	}
	else
	{
		printf("Virus sign copied successfully, found in VirusSign");
	}

	fclose(infectedFile);
	getchar();
	return 0;
}


/* ___createCopy___
Creates copy of virus sign from an infected file.
Input: infected file, start point of the virus in file, end point, name of the sign file.
Output: success - 1, error - 0.
*/
int createCopy(FILE* infectedFile, int startPoint, int endPoint, char* copyName)
{
	int i = 0;
	char buffer[1] = { 0 };
	FILE* copyFile = fopen(copyName, "wb");
	if (!copyFile)
	{
		return 0;
	}
	fseek(infectedFile, startPoint - 1, SEEK_SET); // Goes to the start of the virus sign

	// Copies virus sign
	for(i = 0; i < (endPoint - startPoint + 1); i++)
	{
		fread(buffer, 1, 1, infectedFile);
		fwrite(buffer, 1, 1, copyFile);
	}

	fclose(copyFile);
	return 1;
}