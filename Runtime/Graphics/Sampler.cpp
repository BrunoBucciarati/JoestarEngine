#include "Sampler.h"
namespace Joestar {
    void Sampler::InsertAllHash()
    {
        HashInsert(mMagFilter);
        HashInsert(mMinFilter);
        HashInsert(mMipMapFilter);
        HashInsert(mAddressModeU);
        HashInsert(mAddressModeV);
        HashInsert(mAddressModeW);
        HashInsert(bAnisotrophy);
        HashInsert(mMaxAnisotrophy);
        HashInsert(bCompare);
        HashInsert(mCompareOp);
        HashInsert(mMinLod);
        HashInsert(mMaxLod);
        HashInsert(mMipLodBias);
    }
}