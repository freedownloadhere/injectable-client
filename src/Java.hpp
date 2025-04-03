#pragma once

#include <chrono>
#include <iostream>
#include <thread>

#include "jni.h"

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
	jmethodID mSwingItem = nullptr;

	jfieldID fC02PacketUseEntityAttack = nullptr;
	jfieldID fIsDead = nullptr;
	jfieldID fLoadedEntityList = nullptr;
	jfieldID fSendQueue = nullptr;
	jfieldID fThePlayer = nullptr;
	jfieldID fTheWorld = nullptr;
	
	jobject oC02PacketUseEntityAttack = nullptr;

	bool init() {
		if (JNI_GetCreatedJavaVMs(&jvm, 1, nullptr) != JNI_OK) {
			std::cerr << "[Java] Error: could not get created Java VMs\n";
			return false;
		}

		if (jvm->AttachCurrentThread((void**)&env, nullptr) != JNI_OK) {
			std::cerr << "[Java] Error: could not attach current thread\n";
			return false;
		}

		cC02PacketUseEntity = env->FindClass("in");
		cC02PacketUseEntityAction = env->FindClass("in$a");
		cEntity = env->FindClass("pk");
		cEntityLivingBase = env->FindClass("pr");
		cEntityPlayerSP = env->FindClass("bew");
		cList = env->FindClass("java/util/List");
		cMinecraft = env->FindClass("ave");
		cNetHandlerPlayClient = env->FindClass("bcy");
		cObject = env->FindClass("java/lang/Object");
		cVec3 = env->FindClass("aui");
		cWorldClient = env->FindClass("bdb");

		mAddToSendQueue = env->GetMethodID(cNetHandlerPlayClient, "a", "(Lff;)V");
		mC02PacketUseEntity = env->GetMethodID(cC02PacketUseEntity, "<init>", "(Lpk;Lin$a;)V");
		mDistanceTo = env->GetMethodID(cVec3, "f", "(Laui;)D");
		mEquals = env->GetMethodID(cObject, "equals", "(Ljava/lang/Object;)Z");
		mGetMinecraft = env->GetStaticMethodID(cMinecraft, "A", "()Lave;");
		mGetPositionVector = env->GetMethodID(cEntity, "d", "()Laui;");
		mListGet = env->GetMethodID(cList, "get", "(I)Ljava/lang/Object;");
		mListSize = env->GetMethodID(cList, "size", "()I");
		mSwingItem = env->GetMethodID(cEntityLivingBase, "bw", "()V");

		fC02PacketUseEntityAttack = env->GetStaticFieldID(cC02PacketUseEntityAction, "b", "Lin$a;");
		fIsDead = env->GetFieldID(cEntity, "I", "Z");
		fLoadedEntityList = env->GetFieldID(cWorldClient, "f", "Ljava/util/List;");
		fSendQueue = env->GetFieldID(cEntityPlayerSP, "a", "Lbcy;");
		fThePlayer = env->GetFieldID(cMinecraft, "h", "Lbew;");
		fTheWorld = env->GetFieldID(cMinecraft, "f", "Lbdb;");

		oC02PacketUseEntityAttack = env->GetStaticObjectField(cC02PacketUseEntityAction, fC02PacketUseEntityAttack);

		return true;
	}
}
