#pragma once
#include <Windows.h>
#include <iostream>
#include <tchar.h>
using namespace std;

BOOL DeleteDirectory(LPCTSTR DirectoryFullPath);

BOOL CreateDirectory(LPCTSTR DirectoryFullPath);