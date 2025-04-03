#include <Windows.h>
#include <thread>
#include <stdio.h>
#include <chrono>
#include <iostream>

#include "jni.h"

void DoStuff(JNIEnv* env) {
	jclass cC02PacketUseEntity			= env->FindClass("in");
	jclass cC02PacketUseEntityAction	= env->FindClass("in$a");
	jclass cEntity						= env->FindClass("pk");
	jclass cEntityLivingBase			= env->FindClass("pr");
	jclass cEntityPlayerSP				= env->FindClass("bew");
	jclass cList						= env->FindClass("java/util/List");
	jclass cMinecraft					= env->FindClass("ave");
	jclass cNetHandlerPlayClient		= env->FindClass("bcy");
	jclass cObject						= env->FindClass("java/lang/Object");
	jclass cVec3						= env->FindClass("aui");
	jclass cWorldClient					= env->FindClass("bdb");

	jmethodID mAddToSendQueue			= env->GetMethodID(cNetHandlerPlayClient, "a", "(Lff;)V");
	jmethodID mC02PacketUseEntity		= env->GetMethodID(cC02PacketUseEntity, "<init>", "(Lpk;Lin$a;)V");
	jmethodID mDistanceTo				= env->GetMethodID(cVec3, "f", "(Laui;)D");
	jmethodID mEquals					= env->GetMethodID(cObject, "equals", "(Ljava/lang/Object;)Z");
	jmethodID mGetMinecraft				= env->GetStaticMethodID(cMinecraft, "A", "()Lave;");
	jmethodID mGetPositionVector		= env->GetMethodID(cEntity, "d", "()Laui;");
	jmethodID mListGet					= env->GetMethodID(cList, "get", "(I)Ljava/lang/Object;");
	jmethodID mListSize					= env->GetMethodID(cList, "size", "()I");
	
	jfieldID fC02PacketUseEntityAttack	= env->GetStaticFieldID(cC02PacketUseEntityAction, "b", "Lin$a;");
	jfieldID fIsDead					= env->GetFieldID(cEntity, "I", "Z");
	jfieldID fLoadedEntityList			= env->GetFieldID(cWorldClient, "f", "Ljava/util/List;");
	jfieldID fSendQueue					= env->GetFieldID(cEntityPlayerSP, "a", "Lbcy;");
	jfieldID fThePlayer					= env->GetFieldID(cMinecraft, "h", "Lbew;");
	jfieldID fTheWorld					= env->GetFieldID(cMinecraft, "f", "Lbdb;");

	// Enum instance C02PacketUseEntity.Action.ATTACK
	jobject oC02PacketUseEntityAttack	= env->GetStaticObjectField(cC02PacketUseEntityAction, fC02PacketUseEntityAttack);
	
	std::cout << "Starting Killaura..\n";

	const double MaxDistance = 3.0;

	while (!GetAsyncKeyState(VK_OEM_3)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		jobject oMinecraft = env->CallStaticObjectMethod(cMinecraft, mGetMinecraft);
		if (oMinecraft == nullptr) continue;

		jobject oThePlayer = env->GetObjectField(oMinecraft, fThePlayer);
		if (oThePlayer == nullptr) continue;

		jobject oTheWorld = env->GetObjectField(oMinecraft, fTheWorld);
		if (oTheWorld == nullptr) continue;

		jobject oPlayerVec = env->CallObjectMethod(oThePlayer, mGetPositionVector);
		jobject oSendQueue = env->GetObjectField(oThePlayer, fSendQueue);
		jobject oLoadedEntityList = env->GetObjectField(oTheWorld, fLoadedEntityList);

		jint oListSize = env->CallIntMethod(oLoadedEntityList, mListSize);

		for (int i = 0; i < oListSize; i++) {
			jobject oEntity = env->CallObjectMethod(oLoadedEntityList, mListGet, i);

			jboolean oIsEntityLiving = env->IsInstanceOf(oEntity, cEntityLivingBase);
			if (!oIsEntityLiving) continue;

			jboolean oIsEntityYou = env->CallBooleanMethod(oEntity, mEquals, oThePlayer);
			if (oIsEntityYou) continue;

			jboolean oIsEntityDead = env->GetBooleanField(oEntity, fIsDead);
			if (oIsEntityDead) continue;

			jobject oEntityVec = env->CallObjectMethod(oEntity, mGetPositionVector);

			jdouble oDistance = env->CallDoubleMethod(oEntityVec, mDistanceTo, oPlayerVec);
			if (oDistance > MaxDistance) continue;

			jobject oAttackPacket = env->NewObject(cC02PacketUseEntity, mC02PacketUseEntity, oEntity, oC02PacketUseEntityAttack);

			env->CallVoidMethod(oSendQueue, mAddToSendQueue, oAttackPacket);

			env->DeleteLocalRef(oAttackPacket);
		}
	}
}

void MainButForReal(HINSTANCE instance) {
	FILE* in = nullptr, *out = nullptr, *err = nullptr;
	AllocConsole();
	SetConsoleOutputCP(CP_UTF8);
	freopen_s(&in, "CONIN$", "r", stdin);
	freopen_s(&out, "CONOUT$", "w", stdout);
	freopen_s(&err, "CONOUT$", "w", stderr);

	JavaVM* jvm = nullptr;
	JNIEnv* env = nullptr;

	if (JNI_GetCreatedJavaVMs(&jvm, 1, nullptr) != JNI_OK) {
		std::cerr << "Error: could not get created Java VMs\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		goto finish;
	}

	if (jvm->AttachCurrentThread((void**)&env, nullptr) != JNI_OK) {
		std::cerr << "Error: could not attach current thread\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		goto finish;
	}

	DoStuff(env);

finish:
	if(in) fclose(in);
	if(out) fclose(out);
	if(err) fclose(err);
	FreeConsole();
	FreeLibrary(instance);
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
	if (reason != DLL_PROCESS_ATTACH)
		return FALSE;

	std::thread mainThread(MainButForReal, instance);
	mainThread.detach();

	return TRUE;
}