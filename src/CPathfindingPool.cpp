/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: CPathfindingPool.cpp
	- Author(s): Freaksken

  =========================================*/

#include "Main.hpp"

CPathfindingPool::~CPathfindingPool()
{
	if (m_bInitialized) {
		// Destroy the thread pool workers
		m_Workers.clear();

		// Reset the current worker
		m_iCurrentWorkerIndex = 0;

		// Mark the thread pool uninitialized
		m_bInitialized = false;
	}
}

void CPathfindingPool::Initialize(int iSize)
{
	if (!m_bInitialized) {
		// Create the thread pool workers
		for (int i = 0; i < iSize; i++) {
			m_Workers.push_back(new CPathfindingWorker());
		}

		// Reset the current worker
		m_iCurrentWorkerIndex = 0;

		// Mark the thread pool initialized
		m_bInitialized = true;
	}
}

bool CPathfindingPool::IsInitialized()
{
	return m_bInitialized;
}

void CPathfindingPool::QueueRequest(CPathfindingRequest *pRequest)
{
	if (m_bInitialized) {
		// Queue the request for the current worker
		m_Workers[m_iCurrentWorkerIndex]->QueueRequest(pRequest);

		// Advance the current worker
		m_iCurrentWorkerIndex++;
		if (m_iCurrentWorkerIndex == m_Workers.size()) {
			m_iCurrentWorkerIndex = 0;
		}
	}
}