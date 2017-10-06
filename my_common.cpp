/*
全局的常用函数，包括：

创建 / 删除文件夹
分配 / 释放内存
*/
#include "my_common.h"
#include <Windows.h>
#include <windowsx.h>


	//分配内存
int Common_Tools::GetMemory(void **pOut, int _size)
	{
		BYTE *p;
		if (NULL != *pOut)
		{
		GlobalFreePtr(*pOut);
		}
		p = (LPBYTE)GlobalAllocPtr(GHND, _size);
		*pOut = p;
		if (NULL == p)
		{
		GlobalFreePtr(p);
		p = NULL;
		return -1;
		}

		return 0;
	}

	//释放内存
int Common_Tools::FreeMemory(void **p)
	{
		if (NULL != *p)
		{
		GlobalFreePtr(*p);
		}
		*p = NULL;

		return 0;
	}

