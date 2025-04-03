#pragma once

#include <thread>

#include "Java.hpp"

namespace Killaura {
	const double MaxDistance = 3.0;

	static bool goodEntityCheck(jobject oEntity, jobject oThePlayer) {
		jboolean oIsEntityLiving = Java::env->IsInstanceOf(oEntity, Java::cEntityLivingBase);
		if (!oIsEntityLiving) return false;

		jboolean oIsEntityYou = Java::env->CallBooleanMethod(oEntity, Java::mEquals, oThePlayer);
		if (oIsEntityYou) return false;

		jboolean oIsEntityDead = Java::env->GetBooleanField(oEntity, Java::fIsDead);
		if (oIsEntityDead) return false;

		return true;
	}

	static bool distanceCheck(jobject oEntity, jobject oPlayerVec) {
		jobject oEntityVec = Java::env->CallObjectMethod(oEntity, Java::mGetPositionVector);

		jdouble oDistance = Java::env->CallDoubleMethod(oEntityVec, Java::mDistanceTo, oPlayerVec);
		if (oDistance > MaxDistance) return false;
	}

	static void simulateAttack(jobject oEntity, jobject oThePlayer, jobject oSendQueue) {
		jobject oAttackPacket = Java::env->NewObject(
			Java::cC02PacketUseEntity, Java::mC02PacketUseEntity, 
			oEntity, Java::oC02PacketUseEntityAttack
		);

		Java::env->CallVoidMethod(oThePlayer, Java::mSwingItem);

		Java::env->CallVoidMethod(oSendQueue, Java::mAddToSendQueue, oAttackPacket);

		Java::env->DeleteLocalRef(oAttackPacket);
	}

	void update() {
		jobject oMinecraft = Java::env->CallStaticObjectMethod(Java::cMinecraft, Java::mGetMinecraft);
		if (oMinecraft == nullptr) return;

		jobject oThePlayer = Java::env->GetObjectField(oMinecraft, Java::fThePlayer);
		if (oThePlayer == nullptr) return;

		jobject oTheWorld = Java::env->GetObjectField(oMinecraft, Java::fTheWorld);
		if (oTheWorld == nullptr) return;

		jobject oPlayerVec = Java::env->CallObjectMethod(oThePlayer, Java::mGetPositionVector);
		jobject oSendQueue = Java::env->GetObjectField(oThePlayer, Java::fSendQueue);
		jobject oLoadedEntityList = Java::env->GetObjectField(oTheWorld, Java::fLoadedEntityList);

		jint oListSize = Java::env->CallIntMethod(oLoadedEntityList, Java::mListSize);

		for (int i = 0; i < oListSize; i++) {
			jobject oEntity = Java::env->CallObjectMethod(oLoadedEntityList, Java::mListGet, i);

			if (!goodEntityCheck(oEntity, oThePlayer)) continue;

			if (!distanceCheck(oEntity, oPlayerVec)) continue;

			simulateAttack(oEntity, oThePlayer, oSendQueue);
		}
	}
}
