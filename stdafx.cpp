// stdafx.cpp : 只包括标准包含文件的源文件
// SDKDEMO.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"



int GetBitPos( unsigned long num)
{
	int test = 0;
	int i = -1;
	if(num == 0)
		return i;

	
	while (!test)
	{
		i++;
		test = (num >> i) & 0x1;
	}
	return i;
}
