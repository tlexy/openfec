#ifndef AV_FEC_SYMBOL_H
#define AV_FEC_SYMBOL_H

struct FecSymbolInUnit
{
	void* buf;
	int len;
	int pos;//解码时才有用
};

struct FecSymbolOutUnit
{
	void* buf;
	int len;
	int ori_len;
};

#endif