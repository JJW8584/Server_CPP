#pragma once

struct JobData
{
	JobData(weak_ptr<JobQueue> owner, JobRef job) : owner(owner), job(job)
	{}

	weak_ptr<JobQueue> owner;
	JobRef job;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return executeTick > other.executeTick;
	}

	uint64 executeTick = 0;
	JobData* jobData = nullptr; // 순서가 바뀔때마다 복사하면 Ref카운트에 영향을 주고 속도도 느려짐
};

//-----------
//	JobTimer
//-----------

class JobTimer
{
public:
	void Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job);
	void Distribute(uint64 now);
	void Clear();

private:
	USE_LOCK;
	PriorityQueue<TimerItem> _items;
	Atomic<bool> _distributing = false;
};