#pragma once
#define REGISTER_OBJECT_ROOT(CLASS)\
	public:\
		static const char* GetTypeStatic() {return #CLASS;}\
		static uint32_t GetClassID() {return ClassID(CLASS);}\
	protected:\
		~CLASS();\
	public:

#define REGISTER_OBJECT(CLASS, PARENT)\
	public:\
		static const char* GetTypeStatic() {return #CLASS;}\
		static uint32_t GetClassID() {return ClassID(CLASS);}\
	typedef PARENT Super;\
	protected:\
		~CLASS();\
	public:
	
#define REGISTER_SUBSYSTEM(CLASS) REGISTER_OBJECT(CLASS, SubSystem);

#define NEW_OBJECT(CLASS)\
	new CLASS(mContext)

#define InitSubSystem(CLASS, CONTEXT) \
	CLASS* ptr##CLASS = new CLASS(CONTEXT);\
	SubSystem* t##CLASS = (SubSystem*)ptr##CLASS;\
	CONTEXT->subSystems[ClassID(CLASS)] = t##CLASS;

#define GET_SET(NAME, TYPE) \
public: \
	const TYPE& Get##NAME() {return m_##NAME;} \
	Set##NAME(TYPE val) {m_##NAME = val;} \
private:\
	TYPE m_##NAME;