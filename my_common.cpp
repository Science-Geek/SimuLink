/*
ȫ�ֵĳ��ú�����������

���� / ɾ���ļ���
���� / �ͷ��ڴ�
*/
#include "my_common.h"
#include <Windows.h>
#include <windowsx.h>


	//�����ڴ�
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

	//�ͷ��ڴ�
int Common_Tools::FreeMemory(void **p)
	{
		if (NULL != *p)
		{
		GlobalFreePtr(*p);
		}
		*p = NULL;

		return 0;
	}

