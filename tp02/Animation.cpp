#include "Animation.h"

#include "Application.h"

Animation::Animation(bool isLooping, float duration) : m_isPlaying(false), m_isPaused(false), m_pauseTime(0.f), m_beginTime(0.f), m_pauseDuration(0.f), m_duration(duration), m_isLooping(isLooping)
{
}


Animation::~Animation()
{
}

void Animation::play()
{
	if (m_isPaused)
	{
		m_pauseDuration += Application::get().getTime() - m_pauseTime;
	}
	else
	{
		m_beginTime = Application::get().getTime();
	}

	m_isPaused = false;
	m_isPlaying = true;
}

void Animation::pause()
{
	m_isPaused = true;
	m_pauseTime = Application::get().getTime();
}

void Animation::stop()
{
	m_isPlaying = true;
	m_isPaused = false;
	m_beginTime = 0;
	m_pauseDuration = 0;
	m_pauseTime = 0;
}

float Animation::getValue() const
{

	float clampedElapsedTime = m_duration == 0.f ? 0.f : getElapsedTime() / m_duration;

	return m_spline.get(clampedElapsedTime);
}

float Animation::getElapsedTime() const
{
	if (m_duration == 0)
		return 0;

	float elapsedTime = 0;

	if (m_isPaused)
		elapsedTime = m_pauseTime;
	else
		elapsedTime = Application::get().getTime() - m_beginTime - m_pauseDuration;

	if (m_isLooping)
	{
		while (elapsedTime > m_duration)
		{
			elapsedTime -= m_duration;
		}
	}
	else
	{
		if (elapsedTime > m_duration)
		{
			elapsedTime = m_duration;
		}
	}

	return elapsedTime;
}

Math::BSpline<float>& Animation::getSpline()
{
	return m_spline;
}

float Animation::getDuration() const
{
	return m_duration;
}

void Animation::setDuration(float duration)
{
	m_duration = duration;
}

bool Animation::getIsLooping() const
{
	return m_isLooping;
}

void Animation::setIsLooping(bool isLooping)
{
	m_isLooping = isLooping;
}
