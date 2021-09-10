#pragma once
#include <stdint.h>
#define REGISTER_OBJECT_ROOT(CLASS)\
	public:\
		static const char* GetTypeStatic() {return #CLASS;}\
		static uint32_t GetClassIDStatic() {return ClassID(CLASS);}\
		virtual uint32_t GetClassID() {return ClassID(CLASS);} \
	protected:\
		virtual ~CLASS();\
	public:

#define REGISTER_OBJECT(CLASS, PARENT)\
	public:\
		static const char* GetTypeStatic() {return #CLASS;}\
		static uint32_t GetClassIDStatic() {return ClassID(CLASS);}\
		virtual uint32_t GetClassID() {return ClassID(CLASS);} \
	typedef PARENT Super;\
	protected:\
		virtual ~CLASS();\
	public:
	
#define REGISTER_SUBSYSTEM(CLASS) REGISTER_OBJECT(CLASS, SubSystem);
#define REGISTER_COMPONENT(CLASS) \
	REGISTER_OBJECT(CLASS, Component); \
	explicit CLASS(EngineContext* ctx, GameObject* obj) : Super(ctx, obj) {Init();}

#define NEW_OBJECT(CLASS)\
	new CLASS(mContext)

#define DELETE_OBJECT(obj)\
	obj->Destroy();

#define NEW_OBJECT_PLACEMENT(CLASS, P)\
	new (P)CLASS(mContext)

#define InitSubSystem(CLASS, CONTEXT) \
	CLASS* ptr##CLASS = new CLASS(CONTEXT);\
	SubSystem* t##CLASS = (SubSystem*)ptr##CLASS;\
	CONTEXT->subSystems[ClassID(CLASS)] = t##CLASS;

#define GET_SET(NAME, TYPE) \
	const TYPE& Get##NAME() {return NAME;} \
	void Set##NAME(TYPE val) {NAME = val;}

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
