#pragma once
#include "pch.hpp"

#include "java/Java.hpp"

class Killaura {
public:
	void update() const;

private:
	const double m_maxDistance = 3.0;

	bool m_goodEntityCheck(jobject oEntity, jobject oThePlayer) const;

	bool m_distanceCheck(jobject oEntityVec, jobject oPlayerVec) const;

	bool m_raycastCheck(jobject oTheWorld, jobject oEntityVec, jobject oPlayerVec) const;

	void m_simulateAttack(jobject oEntity, jobject oThePlayer, jobject oSendQueue) const;
};