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
	kClassIDCount
};

#undef DefineClassID
#endif