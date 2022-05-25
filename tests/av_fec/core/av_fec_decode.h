#ifndef AV_FEC_DECODE_H
#define AV_FEC_DECODE_H


extern "C" {
#include <lib_common/of_openfec_api.h>
}
#include "fec_symbol_def.h"
#include <memory>
#include <list>

class AvFecDecode
{
public:
	AvFecDecode(int src_count, int repair_count);

	bool add_symbols_and_decode(const std::shared_ptr<std::list<FecSymbolInUnit>> symbols);

	void get_decode_symbols(std::list<FecSymbolOutUnit>& symbols);

	~AvFecDecode();
private:
	bool fec_calc();
	void destory();
private:
	of_codec_id_t _codec_id{ OF_CODEC_REED_SOLOMON_GF_2_M_STABLE };
	of_rs_2_m_parameters_t _rs_param;
	of_session_t* _sess;
	int _src_count;
	int _repair_count;
	int _symbol_num;
	void** _src_symbol{nullptr};
	std::shared_ptr<std::list<FecSymbolInUnit>> _enc_symbols;
};

#endif