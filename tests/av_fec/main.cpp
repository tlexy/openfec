#include <iostream>
#include <vector>
#include "core/av_fec.h"
#include "core/av_fec_decode.h"

void decode(int src_count, int repair_count,
	const std::shared_ptr<std::list<FecSymbolInUnit>> symbols)
{
	auto decoder = std::make_shared<AvFecDecode>(src_count, repair_count);
	decoder->add_symbols_and_decode(symbols);

	std::list<FecSymbolOutUnit> ori_symbols;
	decoder->get_decode_symbols(ori_symbols);
	int a = 1;
}

int main()
{
	auto af = std::make_shared<AvFec>(8, 4);

	//int ss[] = {10, 11, 12};
	int ss = 10;
	std::shared_ptr<std::list<FecSymbolInUnit>> fsu = std::make_shared<std::list<FecSymbolInUnit>>();
	for (int i = 0; i < 8; ++i)
	{
		FecSymbolInUnit unit;
		unit.len = ss++;
		unit.buf = calloc(unit.len, 1);
		memset(unit.buf, 'a' + i, unit.len);
		fsu->push_back(unit);
	}

	bool flag = af->add_symbols(fsu);
	flag = af->fec_calc();

	std::list<FecSymbolOutUnit> outs;
	af->get_enc_symbols(outs);
	for (auto it = outs.begin(); it != outs.end(); ++it)
	{
		std::cout << "ori_len: " << it->ori_len << "\tenc_len: " << it->len << std::endl;
	}

	//decode...
	std::vector<int> sel = {0, 1, 0, 3, 4, 5, 0, 0, 8, 9, 10, 0};
	std::shared_ptr<std::list<FecSymbolInUnit>> ori_symbols = std::make_shared<std::list<FecSymbolInUnit>>();
	int pos = 0;
	for (auto it = outs.begin(); it != outs.end(); ++it)
	{
		if (pos == sel[pos])
		{
			FecSymbolInUnit unit;
			unit.buf = it->buf;
			unit.len = it->len;
			unit.pos = pos;
			ori_symbols->push_back(unit);
		}
		++pos;
	}

	decode(8, 4, ori_symbols);

	std::cin.get();

	return 0;
}