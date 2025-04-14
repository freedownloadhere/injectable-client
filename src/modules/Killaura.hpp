#pragma once
#include "pch.hpp"

#include "java/Java.hpp"

class Killaura {
public:
	void update() {
		const auto& java = Java::getInstance();

		jobject oMinecraft = java.env->CallStaticObjectMethod(java.cMinecraft, java.mGetMinecraft);
		if (oMinecraft == nullptr) return;

		jobject oThePlayer = java.env->GetObjectField(oMinecraft, java.fThePlayer);
		if (oThePlayer == nullptr) return;

		jobject oTheWorld = java.env->GetObjectField(oMinecraft, java.fTheWorld);
		if (oTheWorld == nullptr) return;

		jobject oPlayerVec = java.env->CallObjectMethod(oThePlayer, java.mGetPositionVector);
		oPlayerVec = java.env->CallObjectMethod(oPlayerVec, java.mVec3Add, java.oEyeHeightVector);

		jobject oSendQueue = java.env->GetObjectField(oThePlayer, java.fSendQueue);
		jobject oLoadedEntityList = java.env->GetObjectField(oTheWorld, java.fLoadedEntityList);

		jint oListSize = java.env->CallIntMethod(oLoadedEntityList, java.mListSize);

		for (int i = 0; i < oListSize; i++) {
			jobject oEntity = java.env->CallObjectMethod(oLoadedEntityList, java.mListGet, i);
			if (!m_goodEntityCheck(oEntity, oThePlayer)) continue;

			jobject oEntityVec = java.env->CallObjectMethod(oEntity, java.mGetPositionVector);
			oEntityVec = java.env->CallObjectMethod(oEntityVec, java.mVec3Add, java.oEyeHeightVector);
			if (!m_distanceCheck(oEntityVec, oPlayerVec)) continue;

			if (!m_raycastCheck(oTheWorld, oEntityVec, oPlayerVec)) continue;

			m_simulateAttack(oEntity, oThePlayer, oSendQueue);
		}
	}

private:
	const double m_maxDistance = 3.0;

	bool m_goodEntityCheck(jobject oEntity, jobject oThePlayer) {
		const auto& java = Java::getInstance();

		jboolean oIsEntityLiving = java.env->IsInstanceOf(oEntity, java.cEntityLivingBase);
		if (!oIsEntityLiving) return false;

		jboolean oIsEntityYou = java.env->IsSameObject(oEntity, oThePlayer);
		if (oIsEntityYou) return false;

		jboolean oIsEntityDead = java.env->GetBooleanField(oEntity, java.fIsDead);
		if (oIsEntityDead) return false;

		return true;
	}

	bool m_distanceCheck(jobject oEntityVec, jobject oPlayerVec) {
		const auto& java = Java::getInstance();

		jdouble oDistance = java.env->CallDoubleMethod(oEntityVec, java.mDistanceTo, oPlayerVec);

		return oDistance <= m_maxDistance;
	}

	bool m_raycastCheck(jobject oTheWorld, jobject oEntityVec, jobject oPlayerVec) {
		const auto& java = Java::getInstance();

		jobject oMop = java.env->CallObjectMethod(oTheWorld, java.mRayTraceBlocks, oPlayerVec, oEntityVec);
		if (oMop == nullptr) return true;

		jobject oTypeOfHit = java.env->GetObjectField(oMop, java.fTypeOfHit);

		jboolean oIsEntityHit = java.env->IsSameObject(oTypeOfHit, java.oMovingObjectPositionTypeEntity);

		return oIsEntityHit;
	}

	void m_simulateAttack(jobject oEntity, jobject oThePlayer, jobject oSendQueue) {
		const auto& java = Java::getInstance();

		jobject oAttackPacket = java.env->NewObject(
			java.cC02PacketUseEntity, java.mC02PacketUseEntity, 
			oEntity, java.oC02PacketUseEntityAttack
		);

		java.env->CallVoidMethod(oThePlayer, java.mSwingItem);

		java.env->CallVoidMethod(oSendQueue, java.mAddToSendQueue, oAttackPacket);

		java.env->DeleteLocalRef(oAttackPacket);
	}
};