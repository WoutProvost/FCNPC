/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: CPathfindingPool.hpp
	- Author(s): Freaksken

  =========================================*/

#ifndef PATHFINDINGPOOL_H
#define PATHFINDINGPOOL_H

class CPathfindingPool
{
public:
	CPathfindingPool() : m_bInitialized(false), m_iCurrentWorkerIndex(0) {}
	~CPathfindingPool();

	void Initialize(int iSize);
	bool IsInitialized();

	void QueueRequest(CPathfindingRequest *pRequest);

private:
	bool m_bInitialized;
	int m_iCurrentWorkerIndex;
	std::vector<CPathfindingWorker *> m_Workers;
};

#endif
