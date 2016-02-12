#pragma once

#include "BSpline.h"

class Animation
{
private:
	bool m_isPlaying;
	bool m_isPaused;
	float m_pauseTime;
	float m_pauseDuration;
	float m_beginTime;
	float m_duration;
	bool m_isLooping;

	Math::BSpline<float> m_spline;

public:
	Animation(bool isLooping = true, float duration = 1.f);
	~Animation();

	void play();
	void pause();
	void stop();
	float getValue() const;
	float getElapsedTime() const;
	Math::BSpline<float>& getSpline();

	float getDuration() const;
	void setDuration(float duration);
	bool getIsLooping() const;
	void setIsLooping(bool isLooping);
};

