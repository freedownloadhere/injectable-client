#pragma once
#include "pch.hpp"

#include "jni.h"
#include "jvmti.h"

#include "JavaException.hpp"

/// <summary>
/// Some naming conventions (prefixes) are used:
/// c for class, m for method ID, f for field ID and o for object.
/// I will try to stick as closely as possible to the original Java names.
/// </summary>
class Java {
public:
	static Java& getInstance();
	void initialize();
	void cleanup();

	JavaVM* jvm = nullptr;
	JNIEnv* env = nullptr;
	jvmtiEnv* tienv = nullptr;

	jclass cC02PacketUseEntity = nullptr;
	jclass cC02PacketUseEntityAction = nullptr;
	jclass cEntity = nullptr;
	jclass cEntityLivingBase = nullptr;
	jclass cEntityPlayerSP = nullptr;
	jclass cList = nullptr;
	jclass cMinecraft = nullptr;
	jclass cMovingObjectPosition = nullptr;
	jclass cMovingObjectPositionType = nullptr;
	jclass cNetHandlerPlayClient = nullptr;
	jclass cObject = nullptr;
	jclass cVec3 = nullptr;
	jclass cWorldClient = nullptr;

	jmethodID mAddToSendQueue = nullptr;
	jmethodID mC02PacketUseEntity = nullptr;
	jmethodID mDistanceTo = nullptr;
	jmethodID mEquals = nullptr;
	jmethodID mGetMinecraft = nullptr;
	jmethodID mGetPositionVector = nullptr;
	jmethodID mListGet = nullptr;
	jmethodID mListSize = nullptr;
	jmethodID mRayTraceBlocks = nullptr;
	jmethodID mSwingItem = nullptr;
	jmethodID mVec3 = nullptr;
	jmethodID mVec3Add = nullptr;

	jfieldID fC02PacketUseEntityAttack = nullptr;
	jfieldID fIsDead = nullptr;
	jfieldID fLoadedEntityList = nullptr;
	jfieldID fMovingObjectPositionTypeEntity = nullptr;
	jfieldID fSendQueue = nullptr;
	jfieldID fThePlayer = nullptr;
	jfieldID fTheWorld = nullptr;
	jfieldID fTypeOfHit = nullptr;
	
	jobject oC02PacketUseEntityAttack = nullptr;
	const double oEyeHeight = 1.625;
	jobject oEyeHeightVector = nullptr;
	jobject oMovingObjectPositionTypeEntity = nullptr;

private:
	static Java m_instance;

	enum Flags {
		NONE = 0,
		STATIC = (1 << 0)
	};

	Java() = default;

	jclass m_findClass(const std::string& path);

	jmethodID m_getMethodID(jclass clazz, const std::string& name, const std::string& sig, Flags flags = Flags::NONE);

	jfieldID m_getFieldID(jclass clazz, const std::string& name, const std::string& sig, Flags flags = Flags::NONE);
};
