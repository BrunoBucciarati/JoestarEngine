#include "PipelineState.h"
namespace Joestar {
    void PipelineLayout::ResizeLayouts(U32 sz)
    {
        mSetLayouts.Resize(sz);
        for (U32 i = 0; i < sz; ++i)
        {
            mSetLayouts[i] = JOJO_NEW(DescriptorSetLayout, MEMORY_GFX_STRUCT);
        }
    }
}