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

	kClassIDCount
};

#undef DefineClassID
#endif