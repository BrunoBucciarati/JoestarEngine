#pragma once
#ifndef _JOESTAR_CLASSID_H
#define _JOESTAR_CLASSID_H

#define ClassID(x)                CLASS_##x
#define	DefineClassID(x, classID)  ClassID(x) = classID,

enum ClassIDType {
	DefineClassID(Object, 0)
	DefineClassID(SubSystem, 1)
	DefineClassID(GlobalConfig, 2)
	DefineClassID(Graphics, 3)
	DefineClassID(FileSystem, 4)
	DefineClassID(HID, 5)
	DefineClassID(Scene, 6)
	DefineClassID(Renderer, 7)
	DefineClassID(Texture, 8)
	DefineClassID(Material, 9)
	DefineClassID(Shader, 10)
	DefineClassID(ShaderParser, 11)
	DefineClassID(ProceduralMesh, 12)
	DefineClassID(Mesh, 13)
	DefineClassID(MemoryManager, 14)
	DefineClassID(ProgramCPU, 15)
	DefineClassID(Component, 16)
	DefineClassID(GameObject, 17)
	DefineClassID(Transform, 18)
	DefineClassID(TimeManager, 19)
	DefineClassID(Texture2D, 20)
	DefineClassID(TextureCube, 21)
	DefineClassID(Light, 22)
	DefineClassID(DirectionalLight, 23)
	DefineClassID(PointLight, 24)
	DefineClassID(SpotLight, 25)
	DefineClassID(LightBatch, 26)
	DefineClassID(Image, 27)
	DefineClassID(Camera, 28)
	DefineClassID(Window, 29)
	DefineClassID(Thread, 30)
	DefineClassID(RenderThread, 31)
	DefineClassID(View, 32)
	DefineClassID(MeshRenderer, 33)
	DefineClassID(VertexBuffer, 34)
	DefineClassID(IndexBuffer, 35)
	DefineClassID(Resource, 36)
	DefineClassID(File, 37)
	DefineClassID(ShaderProgram, 38)
	DefineClassID(UniformBuffer, 39)
	DefineClassID(GPUImage, 40)
	DefineClassID(GPUImageView, 41)
	DefineClassID(MaterialInstance, 41)

	kClassIDCount
};

#undef DefineClassID
#endif