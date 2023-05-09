/*********************************
* Class: MAGSHIMIM C2			 *
* Week:                			 *
* Name:                          *
* Credits:                       *
**********************************/

#pragma warning(disable : 4996)
#include <stdio.h>
#include <string.h>
#include "dirent.h"
#include <stdlib.h>

#define DIR_INDEX 1
#define VIRUS_SIGN_INDEX 2

#define NORMAL_SCAN 0 

// Usually used for scan functions
#define FALSE 0
#define TRUE !0
#define ERROR -1

#define STR_LEN 100

// Used for heuristic(quick) scan
#define DIVIDE_TO_20_PERCENT 5
#define REACH_80_PERCENT 4
#define FIRST_20 2
#define LAST_20 3

void sortResults(char** results, int numberOfFiles);
void printAndWriteLog(FILE* logFile, char** results, int numberOfFiles, char* dirName);
void freeMemory(char** results, int numberOfFiles);
void writeWelcomeLog(FILE* logFile, char* dirName, char* virusFileName);
int fileScan(FILE* file, FILE* virusSign, int startCursor, int endCursor);
void printMenu(char* dirName, char* virusFileName);
int quickScan(char*** p_results, FILE* virusSign, char* dirName);
int normalScan(char*** p_results, FILE* virusSign, char* dirName);
FILE* openLogFile(char* dirName);
char** addResults(int infected, char* filePath, char** results, int numberOfFiles);

int main(int argc, char** argv)
{
	char** results = NULL;
	FILE* virusSign = NULL;
	FILE* logFile = NULL;
	int option = 0; // User option
	int numberOfFiles = 0; // Number of files scanned
	
	virusSign = fopen(argv[VIRUS_SIGN_INDEX], "rb");

	logFile = openLogFile(argv[DIR_INDEX]);

	writeWelcomeLog(logFile, argv[DIR_INDEX], argv[VIRUS_SIGN_INDEX]); // print and log Welcome & details
	printMenu(argv[DIR_INDEX], argv[VIRUS_SIGN_INDEX]);
	
	scanf("%d", &option); // User option
	getchar();
	
	printf("Scanning began...\n");
	printf("This process may take several minutes...\n\n");
	fprintf(logFile, "Scanning option:\n");

	// Scans
	if (option == NORMAL_SCAN)
	{
		fprintf(logFile, "Normal scan\n\n");
		numberOfFiles = normalScan(&results, virusSign, argv[DIR_INDEX]);
	}
	else
	{
		fprintf(logFile, "Quick scan\n\n");
		numberOfFiles = quickScan(&results, virusSign, argv[DIR_INDEX]);
	}

	// Prints sorted list of scanned files and their results
	sortResults(results, numberOfFiles);
	printAndWriteLog(logFile, results, numberOfFiles, argv[DIR_INDEX]);

	freeMemory(results, numberOfFiles);
	fclose(logFile);
	fclose(virusSign);

	getchar();
	return 0;
}


/* ___openLogFile___
Open log file, using the full path.
Input: directory name of the log file.
Output: log file opened.
*/
FILE* openLogFile(char* dirName)
{
	char logFullPath[STR_LEN] = { 0 };
	// Creates full path for open
	strcpy(logFullPath, dirName);
	strcat(logFullPath, "/AntiVirusLog.txt");

	return fopen(logFullPath, "w");
}

/* ___freeMemory___
Frees allocated memory.
Input: Results list(to free), number of files scanned(results count).
Output: None
*/
void freeMemory(char** results, int numberOfFiles)
{
	int i = 0;
	for (i = 0; i < numberOfFiles; i++)
	{
		free(results[i]);
	}
	free(results);

}

/* ___writeWelcomeLog___
Writes welcome message with details to the log file.
Input: log file, directory-to-scan name, virus signature file name.
Output: None
*/
void writeWelcomeLog(FILE* logFile, char* dirName, char* virusFileName)
{
	fprintf(logFile, "Anti-virus began! Welcome!\n\n");
	fprintf(logFile, "Folder to scan:\n%s\n", dirName);
	fprintf(logFile, "Virus signature:\n%s\n\n", virusFileName);

}

/* ___printAndWriteLog___
Prints results both to screen and log file.
Input: log file, results list, number of files scanned(results).
Output: None
*/
void printAndWriteLog(FILE* logFile, char** results, int numberOfFiles, char* dirName)
{
	int i = 0;
	
	printf("Scanning:\n");
	fprintf(logFile, "Results:\n");
	for (i = 0; i < numberOfFiles; i++)
	{
		printf("%s\n", results[i]);
		fprintf(logFile, "%s\n", results[i]);
	}
	printf("Scan completed.\n");
	printf("See log path for results: %s/AntiVirusLog.txt\n", dirName);
}

/* ___sortResults___
Sorts scan results a-z, bubble sort.
Input: Results list to sort, number of files(results).
Output: None
*/
void sortResults(char** results, int numberOfFiles)
{
	int i = 0;
	int j = 0;
	char* tmp = NULL;

	for (i = 0; i < numberOfFiles - 1; i++)
	{
		for (j = 0; j < numberOfFiles - i - 1; j++)
		{
			// The actual sort
			if (strcmp(results[j], results[j + 1]) > 0)
			{
				tmp = results[j];
				results[j] = results[j + 1];
				results[j + 1] = tmp;
			}
		}
	}
}

/* ___fileScan___
Scans file for virus from specific startpoint to specific endpoint.
Input: file to scan, virus signature file, startpoint, endpoint.
Output: result - infected, clean, error.
*/
int fileScan(FILE* file, FILE* virusSign, int startCursor, int endCursor)
{
	int i = 0; // Cursor counter
	char scanBuffer[1] = { 0 };
	char virusBuffer[1] = { 0 };
	int found = FALSE;
	
	if (!virusSign)
	{
		return ERROR;
	}
	fseek(file, startCursor, SEEK_SET); // Moves to startpoint
	while (i < (endCursor - startCursor) && !found)
	{
		// Real scan, reads bytes from scanfile and virus and compares.
		fread(scanBuffer, 1, 1, file);
		fread(virusBuffer, 1, 1, virusSign);

		while (virusBuffer[0] == scanBuffer[0] && !feof(virusSign) && !feof(file))
		{
			// Continue to the next byte
			fread(virusBuffer, 1, 1, virusSign);
			fread(scanBuffer, 1, 1, file);
		}
		if (feof(virusSign))
		{
			found = TRUE;
		}

		fseek(virusSign, 0, SEEK_SET); // Prepare for next virus scan
		i++;
	}
	return found;
}

/* ___normalScan___
Scans all files in specific directory for specific virus signature.
Input: results list to add results into, virus signature file, directory name.
Output: Number of files scanned.
*/
int normalScan(char*** p_results, FILE* virusSign, char* dirName)
{
	FILE* scanFile = NULL;
	DIR* scanDir = NULL;
	struct dirent* dirStruct = NULL;
	int infected = FALSE;
	int fileLength = 0;
	char filePath[STR_LEN] = { 0 };
	int numberOfFiles = 0;

	scanDir = opendir(dirName);
	
	while ((dirStruct = readdir(scanDir)) != NULL)
	{
		// Checks if this is a file. Cant shortly compare to DT_REG thanks to the genius INGInious
		if (dirStruct->d_type != DT_DIR && strcmp(dirStruct->d_name, ".") && strcmp(dirStruct->d_name, "..")) 
		{
			
			// Creates path of the file
			strcpy(filePath, dirName);
			strcat(filePath, "/");
			strcat(filePath, dirStruct->d_name);

			scanFile = fopen(filePath, "rb");
			if (!scanFile)
			{
				infected = ERROR;
			}
			else
			{
				// Checks file length
				fseek(scanFile, 0, SEEK_END);
				fileLength = ftell(scanFile);

				infected = fileScan(scanFile, virusSign, 0, fileLength);
				fclose(scanFile);
			}
			*p_results = addResults(infected, filePath, *p_results, numberOfFiles);	// Updates results
			numberOfFiles++;
		}	
	}
	closedir(scanDir);
	return numberOfFiles;
}

/* ___quickScan___
Uses quick scan to scan all files in a specific directory for viruses.
Input: results list to add results into, virus signature file, directory name.
Output: Number of files scanned.
*/
int quickScan(char*** p_results, FILE* virusSign, char* dirName)
{
	DIR* scanDir = NULL;
	struct dirent* dirStruct = NULL;
	int infected = FALSE;
	int fileLength = 0;
	FILE* scanFile = NULL;
	char filePath[STR_LEN] = { 0 };
	int numberOfFiles = 0;
	int firstPercents20 = 0; // Which cursor position is after the first 20% of the file

	scanDir = opendir(dirName);

	while ((dirStruct = readdir(scanDir)) != NULL)
	{
		if (dirStruct->d_type != DT_DIR && strcmp(dirStruct->d_name, ".") && strcmp(dirStruct->d_name, "..")) // Checks if this is a file
		{

			// Creates path of the file
			strcpy(filePath, dirName);
			strcat(filePath, "/");
			strcat(filePath, dirStruct->d_name);

			scanFile = fopen(filePath, "rb");
			if (!scanFile)
			{
				infected = ERROR;
			}
			else
			{
				fseek(scanFile, 0, SEEK_END);
				fileLength = ftell(scanFile);

				firstPercents20 = fileLength / DIVIDE_TO_20_PERCENT; // Used for heuristic scan
				// Scans first 20 percents
				infected = fileScan(scanFile, virusSign, 0, firstPercents20);
				if (!infected)
				{
					// Scans last 20 percents
					infected = fileScan(scanFile, virusSign, firstPercents20 * REACH_80_PERCENT, fileLength);
					if (!infected)
					{
						// Scans rest of the file
						infected = fileScan(scanFile, virusSign, firstPercents20, firstPercents20 * REACH_80_PERCENT);
					}
					else
					{
						infected = LAST_20;
					}
				}
				else
				{
					infected = FIRST_20;
				}
				fclose(scanFile);
			}
			*p_results = addResults(infected, filePath, *p_results, numberOfFiles); // Updates results
			numberOfFiles++;

		}
	}
	closedir(scanDir);
	return numberOfFiles;
}



/* ___addResults___
Adds result to the results list.
Input: result, file path, results list, num of files scanned.
Output: results list.
*/
char** addResults(int infected, char* filePath, char** results, int numberOfFiles)
{
	char result[STR_LEN] = { 0 };

	strcpy(result, filePath);
	strcat(result, " - ");

	if (infected == ERROR)
	{
		strcat(result, "Error opening file");	
	}
	else if (infected)
	{
		strcat(result, "Infected!");
		if (infected == FIRST_20)
		{
			strcat(result, " (first 20%)");
		}
		else if (infected == LAST_20)
		{
			strcat(result, " (last 20%)");
		}
	}
	else
	{
		strcat(result, "Clean");
	}

	// Add result to results
	results = (char**)realloc(results, (numberOfFiles + 1) * sizeof(char*));
	if (!results)
	{
		printf("Error allocating memory!");
		return NULL;
	}
	results[numberOfFiles] = malloc(strlen(result) + 1); // For NULL too, sizeof(char)is 1
	if (!results[numberOfFiles])
	{
		printf("Error allocating memory!");
		return NULL;
	}

	strcpy(results[numberOfFiles], result);
	return results;
}

/* ___printMenu___
Prints menu to the user.
Input: name of dir to scan, name of virus signature file.
Output: None
*/
void printMenu(char* dirName, char* virusFileName)
{
	printf("Welcome to my Virus Scan!\n\n");
	printf("Folder to scan: %s\n", dirName);
	printf("Virus signature: %s\n\n", virusFileName);
	printf("Press 0 for a normal scan or any other key for a quick scan: ");

}


