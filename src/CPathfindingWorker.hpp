/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: CPathfindingWorker.hpp
	- Author(s): Freaksken

  =========================================*/

#ifndef PATHFINDINGWORKER_H
#define PATHFINDINGWORKER_H

class CPathfindingWorker
{
public:
	CPathfindingWorker();
	~CPathfindingWorker();
	
	void QueueRequest(CPathfindingRequest *pRequest);

private:
	std::atomic<bool> m_bWorkerThreadActive;
	std::thread m_WorkerThread;
	std::condition_variable m_QueueNotifier;
	std::mutex m_QueueNotifierMutex;
	/*std::queue<CPathfindingRequest *> m_Queue; //TODO
	std::mutex m_QueueMutex;*/

	void ThreadedTask();
};

#endif
