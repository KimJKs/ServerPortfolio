#include "pch.h"
#include "Global.h"
#include "JobQueue.h"

shared_ptr<JobQueue> GJobQueue = nullptr;

class Global
{
public:
	Global()
	{
		GJobQueue = make_shared<JobQueue>();
	}
	~Global()
	{
		GJobQueue = nullptr;
	}
} GGlobal;