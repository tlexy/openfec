#include "av_fec_decode.h"
#include <stdlib.h>
#include <string.h>

AvFecDecode::AvFecDecode(int src_count, int repair_count)
	:_src_count(src_count),
	_repair_count(repair_count)
{
	_rs_param.m = 8;

	_rs_param.nb_source_symbols = _src_count;
	_rs_param.nb_repair_symbols = _repair_count;
	//_rs_param->encoding_symbol_length = SYMBOL_SIZE;
}

bool AvFecDecode::add_symbols_and_decode(const std::shared_ptr<std::list<FecSymbolInUnit>> symbols)
{
	if (symbols->size() != _src_count || symbols->size() < 1)
	{
		return false;
	}
	_enc_symbols = symbols;
	int slen = symbols->begin()->len;
	for (auto it = symbols->begin(); it != symbols->end(); ++it)
	{
		if (slen != it->len)
		{
			return false;
		}
	}
	_symbol_num = slen;
	//设置参数
	_rs_param.encoding_symbol_length = slen;
	return fec_calc();
}

bool AvFecDecode::fec_calc()
{
	int ret = 0;
	if ((ret = of_create_codec_instance(&_sess, _codec_id, OF_DECODER, 0)) != OF_STATUS_OK)
	{
		return false;
	}

	if (of_set_fec_parameters(_sess, (of_parameters_t*)&_rs_param) != OF_STATUS_OK)
	{
		return false;
	}
	for (auto it = _enc_symbols->begin(); it != _enc_symbols->end(); ++it)
	{
		if (of_decode_with_new_symbol(_sess, it->buf, it->pos) != OF_STATUS_OK)
		{
			return false;
		}
		if (of_is_decoding_complete(_sess) == true)
		{
			return true;
		}
	}
}

void AvFecDecode::get_decode_symbols(std::list<FecSymbolOutUnit>& symbols)
{
	_src_symbol = (void**)calloc(_src_count, sizeof(void*));
	if (of_get_source_symbols_tab(_sess, _src_symbol) != OF_STATUS_OK)
	{
		return;
	}
	for (int i = 0; i < _src_count; ++i)
	{
		FecSymbolOutUnit unit;
		unit.buf = _src_symbol[i];
		unit.len = _symbol_num;
		symbols.push_back(unit);
	}
}

void AvFecDecode::destory()
{
	if (_src_symbol)
	{
		free(_src_symbol);
	}
}

AvFecDecode::~AvFecDecode()
{
	destory();
}