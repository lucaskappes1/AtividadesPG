#include "ControllableCharacter.h"

void ControllableCharacter::update(float deltaTime)
{
	m_translate += m_velocity;
	m_animTimer += deltaTime;
	if(updateAnimationFromVelocity())
	{ 
		m_frameIndex = 24;
		return;
	}
	if (m_animTimer >= m_frameDuration)
	{
		m_frameIndex = glm::length(m_velocity) < 0.5f ? 24 : m_frameIndex + 1;
		if (m_frameIndex >= (m_animationRow + 1) * 8)
		{
			m_frameIndex = m_animationRow * 8;
		}
		m_animTimer = 0.0f;
	}
}
