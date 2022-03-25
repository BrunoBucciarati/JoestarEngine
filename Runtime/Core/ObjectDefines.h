#pragma once
#define REGISTER_OBJECT_ROOT(CLASS)\
	public:\
		static const char* GetTypeStatic() {return #CLASS;}\
		static uint32_t GetClassIDStatic() {return ClassID(CLASS);}\
		virtual uint32_t GetClassID() {return ClassID(CLASS);} \
		virtual ~CLASS();\
	typedef Object Super;

#define REGISTER_OBJECT(CLASS, PARENT)\
	public:\
		static const char* GetTypeStatic() {return #CLASS;}\
		static uint32_t GetClassIDStatic() {return ClassID(CLASS);}\
		virtual uint32_t GetClassID() {return ClassID(CLASS);} \
		virtual ~CLASS();\
	typedef PARENT Super;
	
#define REGISTER_SUBSYSTEM(CLASS) REGISTER_OBJECT(CLASS, SubSystem);
#define REGISTER_COMPONENT_ROOT(CLASS) \
	REGISTER_OBJECT(CLASS, Component); \
	explicit CLASS(EngineContext* ctx, GameObject* obj) : Super(ctx, obj) {Init();}
#define REGISTER_COMPONENT(CLASS, PARENT) \
	REGISTER_OBJECT(CLASS, PARENT); \
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

#define STR_STRUCT(S) #S

#define GET_SET_STATEMENT_PREFIX_INITVALUE(_TYP, _VAR, _PREFIX, _INITVALUE) \
    private: \
        _TYP _PREFIX##_VAR{_INITVALUE}; \
    public: \
        _TYP Get##_VAR () const { return _PREFIX##_VAR; } \
		void Set##_VAR(_TYP value) { _PREFIX##_VAR = value; }

#define GET_SET_STATEMENT_INITVALUE(_TYP, _VAR, _INITVALUE) \
    GET_SET_STATEMENT_PREFIX_INITVALUE(_TYP, _VAR, m, _INITVALUE)

#define GET_SET_STATEMENT_PREFIX(_TYP, _VAR, _PREFIX) \
	GET_SET_STATEMENT_PREFIX_INITVALUE(_TYP, _VAR, m, )

#define GET_SET_STATEMENT(_TYP, _VAR) \
    GET_SET_STATEMENT_PREFIX(_TYP, _VAR, m)

#define GET_SET_STATEMENT_PREFIX_INITVALUE_REF(_TYP, _VAR, _PREFIX, _INITVALUE) \
	GET_SET_STATEMENT_PREFIX_INITVALUE(_TYP##&, _VAR, _PREFIX, _INITVALUE)

#define GET_SET_STATEMENT_INITVALUE_REF(_TYP, _VAR, _INITVALUE) \
    GET_SET_STATEMENT_PREFIX_INITVALUE_REF(_TYP, _VAR, m, _INITVALUE)

#define GET_SET_STATEMENT_PREFIX_REF(_TYP, _VAR, _PREFIX) \
	GET_SET_STATEMENT_PREFIX_INITVALUE_REF(_TYP, _VAR, m, )

#define GET_SET_STATEMENT_REF(_TYP, _VAR) \
    GET_SET_STATEMENT_PREFIX_REF(_TYP, _VAR, m)