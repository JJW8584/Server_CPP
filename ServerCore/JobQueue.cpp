#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

//------------
//	JobQueue
//------------

void JobQueue::Push(JobRef&& job)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK

	// 첫 번째 job을 넣은 스레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 실행중인 jobQueue가 없을 때
		if (LCurrentJobQueue == nullptr)
		{
			Excute();
		}
		else
		{
			// 여유있는 다른 스레드가 실행하도록 넘김
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

void JobQueue::Excute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
		{
			jobs[i]->Excute();
		}

		// 남은 일이 0개면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			
			// 여유있는 다른 스레드가 실행하도록 넘김
			GGlobalQueue->Push(shared_from_this());
		}
	}
}