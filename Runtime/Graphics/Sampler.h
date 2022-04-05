#pragma once
#include "GPUResource.h"
#include "../Core/ObjectDefines.h"
namespace Joestar {
	class Sampler : public GPUResource
	{
        GET_SET_STATEMENT_INITVALUE(Filter, MagFilter, Filter::LINEAR);
        GET_SET_STATEMENT_INITVALUE(Filter, MinFilter, Filter::LINEAR);
        GET_SET_STATEMENT_INITVALUE(Filter, MipMapFilter, Filter::NEAREST);
        GET_SET_STATEMENT_INITVALUE(SamplerAddressMode, AddressModeU, SamplerAddressMode::REPEAT);
        GET_SET_STATEMENT_INITVALUE(SamplerAddressMode, AddressModeV, SamplerAddressMode::REPEAT);
        GET_SET_STATEMENT_INITVALUE(SamplerAddressMode, AddressModeW, SamplerAddressMode::REPEAT);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, Anisotrophy, b, false);
        GET_SET_STATEMENT_INITVALUE(U32, MaxAnisotrophy, 1);
        GET_SET_STATEMENT_PREFIX_INITVALUE(bool, Compare, b, false);
        GET_SET_STATEMENT_INITVALUE(CompareOp, CompareOp, CompareOp::ALWAYS);
        GET_SET_STATEMENT_INITVALUE(F32, MinLod, 0.0F);
        GET_SET_STATEMENT_INITVALUE(F32, MaxLod, 1.0F);
        GET_SET_STATEMENT_INITVALUE(F32, MipLodBias, 0.0F);
        void InsertAllHash() override;
	};
}