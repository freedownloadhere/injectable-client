#include "pch.hpp"
#include "Java.hpp"

static void JNICALL classFileLoadHook(jvmtiEnv* jvmti_env,
	JNIEnv* jni_env,
	jclass class_being_redefined,
	jobject loader,
	const char* name,
	jobject protection_domain,
	jint class_data_len,
	const unsigned char* class_data,
	jint* new_class_data_len,
	unsigned char** new_class_data
) {
	std::cout << "Loaded class " << name << '\n';
}

Java& Java::getInstance() {
	static Java instance;
	return instance;
}

void Java::initialize() {
	if (JNI_GetCreatedJavaVMs(&jvm, 1, nullptr) != JNI_OK)
		throw JavaException("Could not get Java VM");

	if (jvm->AttachCurrentThread((void**)&env, nullptr) != JNI_OK)
		throw JavaException("Could not attach to current thread");

	if (jvm->GetEnv((void**)&tienv, JVMTI_VERSION_1_2) != JNI_OK)
		throw JavaException("Could not initialize jvmti");

	jvmtiCapabilities capabilities{};
	capabilities.can_retransform_classes = true;
	capabilities.can_retransform_any_class = true;
	capabilities.can_generate_all_class_hook_events = true;
	
	if (tienv->AddCapabilities(&capabilities) != JVMTI_ERROR_NONE)
		throw JavaException("Could not add jvmti capabilities");

	if (tienv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr) != JVMTI_ERROR_NONE)
		throw JavaException("Could not enable event classfile load hook");

	jvmtiEventCallbacks callbacks{};
	callbacks.ClassFileLoadHook = classFileLoadHook;

	if (tienv->SetEventCallbacks(&callbacks, sizeof(jvmtiEventCallbacks)) != JVMTI_ERROR_NONE)
		throw JavaException("Could not set jvmti event callbacks");

	// REMEMBER TO PROPERLY DESTRUCT BRUH

	cC02PacketUseEntity = m_findClass("in");
	cC02PacketUseEntityAction = m_findClass("in$a");
	cEntity = m_findClass("pk");
	cEntityLivingBase = m_findClass("pr");
	cEntityPlayerSP = m_findClass("bew");
	cList = m_findClass("java/util/List");
	cMinecraft = m_findClass("ave");
	cMovingObjectPosition = m_findClass("auh");
	cMovingObjectPositionType = m_findClass("auh$a");
	cNetHandlerPlayClient = m_findClass("bcy");
	cObject = m_findClass("java/lang/Object");
	cVec3 = m_findClass("aui");
	cWorldClient = m_findClass("bdb");

	mAddToSendQueue = m_getMethodID(cNetHandlerPlayClient, "a", "(Lff;)V");
	mC02PacketUseEntity = m_getMethodID(cC02PacketUseEntity, "<init>", "(Lpk;Lin$a;)V");
	mDistanceTo = m_getMethodID(cVec3, "f", "(Laui;)D");
	mEquals = m_getMethodID(cObject, "equals", "(Ljava/lang/Object;)Z");
	mGetMinecraft = m_getMethodID(cMinecraft, "A", "()Lave;", Flags::STATIC);
	mGetPositionVector = m_getMethodID(cEntity, "d", "()Laui;");
	mListGet = m_getMethodID(cList, "get", "(I)Ljava/lang/Object;");
	mListSize = m_getMethodID(cList, "size", "()I");
	mRayTraceBlocks = m_getMethodID(cWorldClient, "a", "(Laui;Laui;)Lauh;");
	mSwingItem = m_getMethodID(cEntityLivingBase, "bw", "()V");
	mVec3 = m_getMethodID(cVec3, "<init>", "(DDD)V");
	mVec3Add = m_getMethodID(cVec3, "e", "(Laui;)Laui;");

	fC02PacketUseEntityAttack = m_getFieldID(cC02PacketUseEntityAction, "b", "Lin$a;", Flags::STATIC);
	fIsDead = m_getFieldID(cEntity, "I", "Z");
	fLoadedEntityList = m_getFieldID(cWorldClient, "f", "Ljava/util/List;");
	fMovingObjectPositionTypeEntity = m_getFieldID(cMovingObjectPositionType, "c", "Lauh$a;", Flags::STATIC);
	fSendQueue = m_getFieldID(cEntityPlayerSP, "a", "Lbcy;");
	fThePlayer = m_getFieldID(cMinecraft, "h", "Lbew;");
	fTheWorld = m_getFieldID(cMinecraft, "f", "Lbdb;");
	fTypeOfHit = m_getFieldID(cMovingObjectPosition, "a", "Lauh$a;");

	oC02PacketUseEntityAttack = env->GetStaticObjectField(cC02PacketUseEntityAction, fC02PacketUseEntityAttack);
	oEyeHeightVector = env->NewObject(cVec3, mVec3, 0.0, oEyeHeight, 0.0);
	oMovingObjectPositionTypeEntity = env->GetStaticObjectField(cMovingObjectPositionType, fMovingObjectPositionTypeEntity);
}

jclass Java::m_findClass(const std::string& path) {
	jclass clazz = env->FindClass(path.c_str());

	if (clazz == nullptr)
		throw JavaException("Could not find class : " + path);

	return clazz;
}

jmethodID Java::m_getMethodID(jclass clazz, const std::string& name, const std::string& sig, Flags flags) {
	const bool isStatic = flags & Flags::STATIC;

	jmethodID method = nullptr;
	if (isStatic) {
		method = env->GetStaticMethodID(clazz, name.c_str(), sig.c_str());
	}
	else {
		method = env->GetMethodID(clazz, name.c_str(), sig.c_str());
	}

	if (method == nullptr) {
		throw JavaException("Method was not found : name " + name + " : signature " + sig + (isStatic ? " : static" : ""));
	}

	return method;
}

jfieldID Java::m_getFieldID(jclass clazz, const std::string& name, const std::string& sig, Flags flags) {
	const bool isStatic = flags & Flags::STATIC;

	jfieldID field = nullptr;
	if (isStatic) {
		field = env->GetStaticFieldID(clazz, name.c_str(), sig.c_str());
	}
	else {
		field = env->GetFieldID(clazz, name.c_str(), sig.c_str());
	}

	if (field == nullptr) {
		throw JavaException("Field was not found : name " + name + " : signature " + sig + (isStatic ? " : static" : ""));
	}

	return field;
}
