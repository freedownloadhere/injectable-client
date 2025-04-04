#pragma once

#include <chrono>
#include <iostream>
#include <thread>
#include <unordered_map>

#include "jni.h"
#include "jvmti.h"

#include "JavaException.hpp"

/// <summary>
/// Some naming conventions (prefixes) are used:
/// c for class, m for method ID, f for field ID and o for object.
/// I will try to stick as closely as possible to the original Java names.
/// </summary>
namespace Java {
	JavaVM* jvm = nullptr;
	JNIEnv* env = nullptr;

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

	enum Flags {
		NONE = 0,
		STATIC = (1 << 0)
	};

	/// <summary>
	/// code by Lefraudeur :
	/// https://github.com/Lefraudeur 
	/// </summary>
	static jclass findClass(const std::string& path) {
		static std::unordered_map<std::string, jclass> classMap;

		if (classMap.empty()) {
			JavaVM* jvm = nullptr;
			env->GetJavaVM(&jvm);
			jvmtiEnv* tienv = nullptr;
			jvm->GetEnv((void**)&tienv, JVMTI_VERSION_1_2);
			jint classCount = 0;
			jclass* classes = nullptr;
			tienv->GetLoadedClasses(&classCount, &classes);
			jclass ClassClass = env->FindClass("java/lang/Class");
			jmethodID getName = env->GetMethodID(ClassClass, "getName", "()Ljava/lang/String;");

			for (int i = 0; i < classCount; ++i) {
				jstring name = (jstring)env->CallObjectMethod(classes[i], getName);
				const char* chars = env->GetStringUTFChars(name, nullptr);
				jsize size = env->GetStringUTFLength(name);
				std::string className(chars, size);
				env->ReleaseStringUTFChars(name, chars);
				for (char& character : className) {
					if (character == '.') 
						character = '/';
				}
				classMap.insert({ className, classes[i] });
			}
			tienv->Deallocate((unsigned char*)classes);
		}

		jclass foundclass = nullptr;

		if (!classMap.count(path)) {
			throw JavaException("Class was not found: " + path);
		}

		foundclass = classMap.at(path);
		return foundclass;
	}

	static jmethodID getMethodID(jclass clazz, const std::string& name, const std::string& sig, Flags flags = Flags::NONE) {
		const bool isStatic = flags & Flags::STATIC;

		jmethodID method = nullptr;
		if (isStatic) {
			method = env->GetStaticMethodID(clazz, name.c_str(), sig.c_str());
		} else {
			method = env->GetMethodID(clazz, name.c_str(), sig.c_str());
		}

		if (method == nullptr) {
			throw JavaException("Method was not found : name " + name + " : signature " + sig + (isStatic ? " : static" : ""));
		}

		return method;
	}

	static jfieldID getFieldID(jclass clazz, const std::string& name, const std::string& sig, Flags flags = Flags::NONE) {
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

	void init() {
		if (JNI_GetCreatedJavaVMs(&jvm, 1, nullptr) != JNI_OK)
			throw JavaException("Could not get Java VM");

		if (jvm->AttachCurrentThread((void**)&env, nullptr) != JNI_OK)
			throw JavaException("Could not attach to current thread");

		cC02PacketUseEntity			= findClass("in");
		cC02PacketUseEntityAction	= findClass("in$a");
		cEntity						= findClass("pk");
		cEntityLivingBase			= findClass("pr");
		cEntityPlayerSP				= findClass("bew");
		cList						= findClass("java/util/List");
		cMinecraft					= findClass("ave");
		cMovingObjectPosition		= findClass("auh");
		cMovingObjectPositionType	= findClass("auh$a");
		cNetHandlerPlayClient		= findClass("bcy");
		cObject						= findClass("java/lang/Object");
		cVec3						= findClass("aui");
		cWorldClient				= findClass("bdb");

		mAddToSendQueue				= getMethodID(cNetHandlerPlayClient, "a", "(Lff;)V");
		mC02PacketUseEntity			= getMethodID(cC02PacketUseEntity, "<init>", "(Lpk;Lin$a;)V");
		mDistanceTo					= getMethodID(cVec3, "f", "(Laui;)D");
		mEquals						= getMethodID(cObject, "equals", "(Ljava/lang/Object;)Z");
		mGetMinecraft				= getMethodID(cMinecraft, "A", "()Lave;", Flags::STATIC);
		mGetPositionVector			= getMethodID(cEntity, "d", "()Laui;");
		mListGet					= getMethodID(cList, "get", "(I)Ljava/lang/Object;");
		mListSize					= getMethodID(cList, "size", "()I");
		mRayTraceBlocks				= getMethodID(cWorldClient, "a", "(Laui;Laui;)Lauh;");
		mSwingItem					= getMethodID(cEntityLivingBase, "bw", "()V");
		mVec3						= getMethodID(cVec3, "<init>", "(DDD)V");
		mVec3Add					= getMethodID(cVec3, "e", "(Laui;)Laui;");

		fC02PacketUseEntityAttack		= getFieldID(cC02PacketUseEntityAction, "b", "Lin$a;", Flags::STATIC);
		fIsDead							= getFieldID(cEntity, "I", "Z");
		fLoadedEntityList				= getFieldID(cWorldClient, "f", "Ljava/util/List;");
		fMovingObjectPositionTypeEntity = getFieldID(cMovingObjectPositionType, "c", "Lauh$a;", Flags::STATIC);
		fSendQueue						= getFieldID(cEntityPlayerSP, "a", "Lbcy;");
		fThePlayer						= getFieldID(cMinecraft, "h", "Lbew;");
		fTheWorld						= getFieldID(cMinecraft, "f", "Lbdb;");
		fTypeOfHit						= getFieldID(cMovingObjectPosition, "a", "Lauh$a;");

		oC02PacketUseEntityAttack		= env->GetStaticObjectField(cC02PacketUseEntityAction, fC02PacketUseEntityAttack);
		oEyeHeightVector				= env->NewObject(cVec3, mVec3, 0.0, oEyeHeight, 0.0);
		oMovingObjectPositionTypeEntity = env->GetStaticObjectField(cMovingObjectPositionType, fMovingObjectPositionTypeEntity);
	}
}
