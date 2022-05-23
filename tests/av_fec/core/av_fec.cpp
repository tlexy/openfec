#include "av_fec.h"
#include <stdlib.h>
#include <string.h>

AvFec::AvFec(int src_count, int repair_count) 
	:_src_count(src_count),
	_repair_count(repair_count)
{
	_enc_symbols_tab = (void**)calloc(src_count + repair_count, sizeof(void*));

	_rs_param.m = 8;

	_rs_param.nb_source_symbols = _src_count;
	_rs_param.nb_repair_symbols = _repair_count;
	//_rs_param->encoding_symbol_length = SYMBOL_SIZE;
}

bool AvFec::add_symbols(const std::shared_ptr<std::list<FecSymbolInUnit>> symbols)
{
	if (symbols->size() != _src_count)
	{
		return false;
	}
	_ori_symbols = symbols;
	//找出最大值
	int max = 0;
	for (auto it = symbols->begin(); it != symbols->end(); ++it)
	{
		if (it->len > max)
		{
			max = it->len;
		}
	}
	int slen = max / 4;
	slen = slen * 4;//必须为32位的整数倍
	if (slen < max)
	{
		slen += 4;
	}
	//设置参数
	_rs_param.encoding_symbol_length = slen;

	//将数据复制到内存buffer
	int index = 0;
	for (auto it = symbols->begin(); it != symbols->end(); ++it)
	{
		_enc_symbols_tab[index++] = calloc(slen, 1);
		memcpy(_enc_symbols_tab[index-1], it->buf, it->len);
	}
	//冗余包申请
	for (int i = index; i < _src_count + _repair_count; ++i)
	{
		_enc_symbols_tab[i] = calloc(slen, 1);
	}
	return true;
}

void AvFec::get_enc_symbols(std::list<FecSymbolOutUnit>& symbols)
{
	auto it = _ori_symbols->begin();
	for (int i = 0; i < _src_count + _repair_count; ++i)
	{
		FecSymbolOutUnit ou;
		ou.len = _rs_param.encoding_symbol_length;
		ou.buf = _enc_symbols_tab[i];
		if (it != _ori_symbols->end())
		{
			ou.ori_len = it->len;
			++it;
		}
		else
		{
			ou.ori_len = 0;//标识是一个冗余包
		}

		symbols.push_back(ou);
	}
}

bool AvFec::fec_calc()
{
	int ret = 0;
	if ((ret = of_create_codec_instance(&_sess, _codec_id, OF_ENCODER, 0)) != OF_STATUS_OK)
	{
		return false;
	}

	if (of_set_fec_parameters(_sess, (of_parameters_t*)&_rs_param) != OF_STATUS_OK)
	{
		return false;
	}
	for (int i = _src_count; i < _src_count + _repair_count; ++i)
	{
		if (of_build_repair_symbol(_sess, _enc_symbols_tab, i) != OF_STATUS_OK)
		{
			return false;
		}
	}
	return true;
}