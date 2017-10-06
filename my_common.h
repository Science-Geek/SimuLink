#pragma once

class Common_Tools
{

public:
	//分配内存
	static int GetMemory(void **pOut, int _size);
	//释放内存
	static int FreeMemory(void **p);

private:

};