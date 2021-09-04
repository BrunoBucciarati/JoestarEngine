#pragma once
#ifndef _JOESTAR_SHADER_DEFS_H_
#define _JOESTAR_SHADER_DEFS_H_

enum ShaderStage {
    kVertexShader = 1 << 0,
    kFragmentShader = 1 << 1,
    kGeometryShader = 1 << 2,
    kComputeShader = 1 << 3
};

#endif