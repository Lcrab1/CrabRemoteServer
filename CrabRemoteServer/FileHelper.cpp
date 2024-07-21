#pragma once
#include "pch.h"
#include "FileHelper.h"




BOOL DeleteDirectory(LPCTSTR DirectoryFullPath)
{
	WIN32_FIND_DATA	v1;
	TCHAR	BufferData[MAX_PATH] = { 0 };

	_stprintf(BufferData, _T("%s\\*.*"), DirectoryFullPath);

	HANDLE FileHandle = FindFirstFile(BufferData, &v1);
	if (FileHandle == INVALID_HANDLE_VALUE) //如果没有找到或查找失败
	{
		return FALSE;
	}
	do
	{
		if (v1.cFileName[0] == _T('.') && _tcslen(v1.cFileName) <= 2)
		{
			continue;
		}
		else
		{
			if (v1.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				TCHAR v2[MAX_PATH];
				_stprintf(v2, _T("%s\\%s"), DirectoryFullPath, v1.cFileName);
				DeleteDirectory(v2);
			}
			else
			{
				TCHAR v2[MAX_PATH];
				_stprintf(v2, "%s\\%s", DirectoryFullPath, v1.cFileName);
				DeleteFile(v2);
			}
		}
	} while (FindNextFile(FileHandle, &v1));

	FindClose(FileHandle); //关闭查找句柄    

	if (!RemoveDirectory(DirectoryFullPath))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CreateDirectory(LPCTSTR DirectoryFullPath)
{

	TCHAR* Travel = NULL;
	TCHAR* BufferData = NULL;
	DWORD DirectoryAttributes;
	__try
	{
		BufferData = (TCHAR*)malloc(sizeof(TCHAR) * (_tcslen(DirectoryFullPath) + 1));

		if (BufferData == NULL)
		{
			return FALSE;
		}

		_tcscpy(BufferData, DirectoryFullPath);

		Travel = BufferData;


		if (*(Travel + 1) == _T(':'))
		{
			Travel++;
			Travel++;
			if (*Travel && (*Travel == _T('\\')))
			{
				Travel++;
			}
		}
		//深层目录
		while (*Travel)
		{
			if (*Travel == _T('\\'))
			{
				*Travel = _T('\0');
				DirectoryAttributes = GetFileAttributes(BufferData);   //查看是否是否目录  目录存在吗
				if (DirectoryAttributes == 0xffffffff)
				{
					if (!CreateDirectory(BufferData, NULL))
					{
						if (GetLastError() != ERROR_ALREADY_EXISTS)
						{
							free(BufferData);
							return FALSE;
						}
					}
				}
				else
				{
					if ((DirectoryAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
					{
						free(BufferData);
						BufferData = NULL;
						return FALSE;
					}
				}

				*Travel = _T('\\');
			}

			Travel = CharNext(Travel);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		if (BufferData != NULL)
		{
			free(BufferData);

			BufferData = NULL;
		}

		return FALSE;
	}

	if (BufferData != NULL)
	{
		free(BufferData);
		BufferData = NULL;
	}
	return TRUE;
}