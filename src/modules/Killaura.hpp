#pragma once
#include "pch.hpp"

#include "java/Java.hpp"

namespace Killaura {
	const double MaxDistance = 3.0;

	static bool goodEntityCheck(jobject oEntity, jobject oThePlayer) {
		jboolean oIsEntityLiving = Java::env->IsInstanceOf(oEntity, Java::cEntityLivingBase);
		if (!oIsEntityLiving) return false;

		jboolean oIsEntityYou = Java::env->IsSameObject(oEntity, oThePlayer);
		if (oIsEntityYou) return false;

		jboolean oIsEntityDead = Java::env->GetBooleanField(oEntity, Java::fIsDead);
		if (oIsEntityDead) return false;

		return true;
	}

	static bool distanceCheck(jobject oEntityVec, jobject oPlayerVec) {
		jdouble oDistance = Java::env->CallDoubleMethod(oEntityVec, Java::mDistanceTo, oPlayerVec);

		return oDistance <= MaxDistance;
	}

	static bool raycastCheck(jobject oTheWorld, jobject oEntityVec, jobject oPlayerVec) {
		jobject oMop = Java::env->CallObjectMethod(oTheWorld, Java::mRayTraceBlocks, oPlayerVec, oEntityVec);
		if (oMop == nullptr) return true;

		jobject oTypeOfHit = Java::env->GetObjectField(oMop, Java::fTypeOfHit);

		jboolean oIsEntityHit = Java::env->IsSameObject(oTypeOfHit, Java::oMovingObjectPositionTypeEntity);

		return oIsEntityHit;
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
		oPlayerVec = Java::env->CallObjectMethod(oPlayerVec, Java::mVec3Add, Java::oEyeHeightVector);

		jobject oSendQueue = Java::env->GetObjectField(oThePlayer, Java::fSendQueue);
		jobject oLoadedEntityList = Java::env->GetObjectField(oTheWorld, Java::fLoadedEntityList);

		jint oListSize = Java::env->CallIntMethod(oLoadedEntityList, Java::mListSize);

		for (int i = 0; i < oListSize; i++) {
			jobject oEntity = Java::env->CallObjectMethod(oLoadedEntityList, Java::mListGet, i);
			if (!goodEntityCheck(oEntity, oThePlayer)) continue;

			jobject oEntityVec = Java::env->CallObjectMethod(oEntity, Java::mGetPositionVector);
			oEntityVec = Java::env->CallObjectMethod(oEntityVec, Java::mVec3Add, Java::oEyeHeightVector);
			if (!distanceCheck(oEntityVec, oPlayerVec)) continue;

			if (!raycastCheck(oTheWorld, oEntityVec, oPlayerVec)) continue;

			simulateAttack(oEntity, oThePlayer, oSendQueue);
		}
	}
}
