#pragma once

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void Stop();
	void Reset();

	float GetSeconds() const;
	long long GetCounts() const;

private:
	long long m_frequency;
	long long m_startTime;
	long long m_stopTime;
};
