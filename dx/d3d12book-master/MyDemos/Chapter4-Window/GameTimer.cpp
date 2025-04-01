#include "GameTimer.h"

GameTimer::GameTimer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mStopTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	long countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

GameTimer::~GameTimer()
{
}

float GameTimer::TotalTime() const
{
	if (mStopped)
	{
		return (float)(mStopTime - mPausedTime - mBaseTime)*mSecondsPerCount;
	}
	else {
		return (float)(mCurrTime - mPausedTime - mBaseTime) * mSecondsPerCount;
	}
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mBaseTime = currentTime;
	mPrevTime = currentTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}

}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		mStopTime = currentTime;
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mCurrTime = currentTime;
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
	mPrevTime = mCurrTime;
	mDeltaTime = max(0.0, mDeltaTime);

}

