#include <iostream>
#include "core/av_fec.h"

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

	std::cin.get();

	return 0;
}