#include "pch.h"
#include "JobQueue.h"

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
		Excute();
	}
}

void JobQueue::Excute()
{
	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
		{
			jobs[i]->Excute();
		}

		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			return;
		}
	}
}
