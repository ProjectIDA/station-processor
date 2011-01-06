#include "stdafx.h"
#include <string>

using namespace std;

static bool bResult;

static void DeleteAllFiles(char* folderPath)
{
 char fileFound[256];
 WIN32_FIND_DATA info;
 HANDLE hp; 
 sprintf(fileFound, "%s\\*.*", folderPath);
 hp = FindFirstFile(fileFound, &info);
 do
    {
       sprintf(fileFound,"%s\\%s", folderPath, info.cFileName);
       DeleteFile(fileFound);
 
    }while(FindNextFile(hp, &info)); 
 FindClose(hp);
}



static void EmptyDirectory_(const char* folderPath)
	{
	char fileFound[256];
	WIN32_FIND_DATA info;
	HANDLE hp; 
	sprintf(fileFound, "%s\\*.*", folderPath);
	hp = FindFirstFile(fileFound, &info);
	do
		{
			if (!((strcmp(info.cFileName, ".")==0)||
              (strcmp(info.cFileName, "..")==0)))
				{
				if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==
                                     FILE_ATTRIBUTE_DIRECTORY)
					{
					string subFolder = folderPath;
					subFolder.append("\\");
					subFolder.append(info.cFileName);
					EmptyDirectory_((char*)subFolder.c_str());
					if(!RemoveDirectory(subFolder.c_str())) bResult=false;

					}
				else
					{
					sprintf(fileFound,"%s\\%s", folderPath, info.cFileName);
					DeleteFile(fileFound);
					}
				}
 
		}while(FindNextFile(hp, &info)); 
	FindClose(hp);
	}

bool EmptyDirectory(const char * folderPath)
	{
	bResult = true;
	EmptyDirectory_(folderPath);
	if(!RemoveDirectory(folderPath)) bResult=false;
	return bResult;
	}
