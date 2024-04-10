#pragma once
#include <windows.h>
#include <iostream>
using namespace std;
//辅助关键段使用
//RAII技术
class _CCriticalSection
{
public:
	_CCriticalSection(CRITICAL_SECTION* CriticalSection);
	~_CCriticalSection();
	void Lock();   //关键段保护
	void Unlock(); //离开关键段
private:
	CRITICAL_SECTION* m_CriticalSection;  //指针   
};

