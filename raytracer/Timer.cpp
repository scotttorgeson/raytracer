#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Timer::Timer()
{
	QueryPerformanceFrequency( ( LARGE_INTEGER* )&m_frequency );

	Reset();
}

Timer::~Timer()
{

}

void Timer::Start()
{
	QueryPerformanceCounter( ( LARGE_INTEGER* )&m_startTime );
}

void Timer::Stop()
{
	QueryPerformanceCounter( ( LARGE_INTEGER* )&m_stopTime );
}

void Timer::Reset()
{
	m_startTime = -1;
	m_stopTime = -1;
}

float Timer::GetSeconds() const
{
	if ( m_startTime < 0 || m_stopTime < 0 )
		return -1.0f;

	return ( float )( m_stopTime - m_startTime ) / ( float )m_frequency;
}

long long Timer::GetCounts() const
{
	if ( m_startTime < 0 || m_stopTime < 0 )
		return -1;

	return m_stopTime - m_startTime;
}