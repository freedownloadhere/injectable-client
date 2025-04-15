// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "pch.hpp"
#include "java/Java.hpp"
#include "mapper/Mapper.hpp"

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

	jvmtiCapabilities capabilities{
		.can_get_bytecodes = true,
		.can_redefine_classes = true,
		.can_redefine_any_class = true,
		.can_generate_all_class_hook_events = true,
		.can_retransform_classes = true,
		.can_retransform_any_class = true,
	};
	
	if (tienv->AddCapabilities(&capabilities) != JVMTI_ERROR_NONE)
		throw JavaException("Could not add jvmti capabilities");

	if (tienv->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr) != JVMTI_ERROR_NONE)
		throw JavaException("Could not enable event classfile load hook");

	m_findClassLoaderOf("Lave;");

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
	mGetMinecraft = m_getMethodID(cMinecraft, "A", "()Lave;");
	mGetPositionVector = m_getMethodID(cEntity, "d", "()Laui;");
	mListGet = m_getMethodID(cList, "get", "(I)Ljava/lang/Object;");
	mListSize = m_getMethodID(cList, "size", "()I");
	mRayTraceBlocks = m_getMethodID(cWorldClient, "a", "(Laui;Laui;)Lauh;");
	mSwingItem = m_getMethodID(cEntityLivingBase, "bw", "()V");
	mVec3 = m_getMethodID(cVec3, "<init>", "(DDD)V");
	mVec3Add = m_getMethodID(cVec3, "e", "(Laui;)Laui;");

	fC02PacketUseEntityAttack = m_getFieldID(cC02PacketUseEntityAction, "b", "Lin$a;");
	fIsDead = m_getFieldID(cEntity, "I", "Z");
	fLoadedEntityList = m_getFieldID(cWorldClient, "f", "Ljava/util/List;");
	fMovingObjectPositionTypeEntity = m_getFieldID(cMovingObjectPositionType, "c", "Lauh$a;");
	fSendQueue = m_getFieldID(cEntityPlayerSP, "a", "Lbcy;");
	fThePlayer = m_getFieldID(cMinecraft, "h", "Lbew;");
	fTheWorld = m_getFieldID(cMinecraft, "f", "Lbdb;");
	fTypeOfHit = m_getFieldID(cMovingObjectPosition, "a", "Lauh$a;");

	oC02PacketUseEntityAttack = env->GetStaticObjectField(cC02PacketUseEntityAction, fC02PacketUseEntityAttack);
	oEyeHeightVector = env->NewObject(cVec3, mVec3, 0.0, oEyeHeight, 0.0);
	oMovingObjectPositionTypeEntity = env->GetStaticObjectField(cMovingObjectPositionType, fMovingObjectPositionTypeEntity);
}

void Java::cleanup() {
	tienv->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, nullptr);
}

void Java::m_findClassLoaderOf(std::string_view qualifiedName) {
	jint classArraySize = 0;
	jclass* classArray = nullptr;
	if (tienv->GetLoadedClasses(&classArraySize, &classArray) != JVMTI_ERROR_NONE)
		throw JavaException("Failed to get the loaded classes");

	bool foundClassLoader = false;

	for (int i = 0; !foundClassLoader && i < classArraySize; ++i) {
		jclass oClass = classArray[i];

		char* classSignature = nullptr;
		if (tienv->GetClassSignature(oClass, &classSignature, nullptr) != JVMTI_ERROR_NONE)
			throw JavaException("Failed to get a class signature");

		if (qualifiedName == classSignature) {
			if (tienv->GetClassLoader(oClass, &m_oClassLoader) != JVMTI_ERROR_NONE)
				throw JavaException("Failed to get Minecraft class loader");
			foundClassLoader = true;
		}

		if (tienv->Deallocate(reinterpret_cast<unsigned char*>(classSignature)) != JVMTI_ERROR_NONE)
			throw JavaException("Failed to deallocate a class signature string");
	}

	if (tienv->Deallocate(reinterpret_cast<unsigned char*>(classArray)) != JVMTI_ERROR_NONE)
		throw JavaException("Failed to deallocate the loaded classes array");

	if (m_oClassLoader == nullptr)
		throw JavaException("Failed to find class loader");

	m_cClass = env->FindClass("java/lang/Class");
	if (m_cClass == nullptr)
		throw JavaException("Failed to get the Class class");

	m_mForName = env->GetStaticMethodID(m_cClass, "forName", "(Ljava/lang/String;ZLjava/lang/ClassLoader;)Ljava/lang/Class;");
	if (m_mForName == nullptr)
		throw JavaException("Failed to get the Class.forName method");
}

jclass Java::m_findClass(const std::string& path) {
	jclass clazz = nullptr;

	clazz = env->FindClass(path.c_str());
	if (clazz != nullptr)
		return clazz;

	jstring oClassName = env->NewStringUTF(path.c_str());
	if (oClassName == nullptr)
		throw JavaException("Failed to create a class name string");

	jboolean oClassInitialize = 0;
	
	clazz = (jclass)env->CallStaticObjectMethod(m_cClass, m_mForName, oClassName, oClassInitialize, m_oClassLoader);
	if (clazz != nullptr) {
		env->DeleteLocalRef(oClassName);
		return clazz;
	}

	throw JavaException("Could not find class : " + path);
}

jmethodID Java::m_getMethodID(jclass clazz, const std::string& name, const std::string& sig) {
	jmethodID method = nullptr;
	if (method = env->GetStaticMethodID(clazz, name.c_str(), sig.c_str()))
		return method;
	if(method = env->GetMethodID(clazz, name.c_str(), sig.c_str()))
		return method;

	throw JavaException("Method was not found : name " + name + " : signature " + sig);
}

jfieldID Java::m_getFieldID(jclass clazz, const std::string& name, const std::string& sig) {
	jfieldID field = nullptr;
	if (field = env->GetStaticFieldID(clazz, name.c_str(), sig.c_str()))
		return field;
	if (field = env->GetFieldID(clazz, name.c_str(), sig.c_str()))
		return field;

	throw JavaException("Field was not found : name " + name + " : signature " + sig);
}
