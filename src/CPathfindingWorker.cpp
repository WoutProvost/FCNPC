/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: CPathfindingWorker.cpp
	- Author(s): Freaksken

  =========================================*/

#include "Main.hpp"

CPathfindingWorker::CPathfindingWorker()
{
	// Initialize the worker
	m_bWorkerThreadActive = true;

	// Start the thread
	m_WorkerThread = std::thread(&CPathfindingWorker::ThreadedTask);
}

CPathfindingWorker::~CPathfindingWorker()
{
	// Signal the thread to terminate
	m_bWorkerThreadActive = false;

	// Signal the condition variable, and unblock the waiting thread to process any remaining requests in the queue
	m_QueueNotifier.notify_one();

	// Wait for the thread to finish
	if (m_WorkerThread.joinable()) {
		m_WorkerThread.join();
	}
}

void CPathfindingWorker::QueueRequest(CPathfindingRequest *pRequest)
{
	// Lock the queue //TODO
	/*std::lock_guard<std::mutex> lock_guard(m_QueueMutex);

	// Queue the request
	m_Queue.push(pRequest);*/

	// Signal the condition variable, and unblock the waiting thread to process any requests in the queue
	m_QueueNotifier.notify_one();
}

// Threaded function
void CPathfindingWorker::ThreadedTask()
{
	// Aquire the notifier lock
	std::unique_lock<std::mutex> lock(m_QueueNotifierMutex);

	// If the thread is not signaled to terminate
	while (m_bWorkerThreadActive) {
		// Release the lock and block this thread until notified to continue by the condition variable
		// When the condition variable gets signaled, resume the thread and aquire the lock again
		// Check if the condition is met, if so continue thread execution, if not wait again
		m_QueueNotifier.wait(lock);

		// Lock the queue //TODO
		/*std::lock_guard<std::mutex> lock_guard(m_QueueMutex);

		// Process any queued requests
		while (!m_Queue.empty()) {
			CPathfindingRequest *pRequest = m_Queue.front();
			m_Queue.pop();

			// Execute the request
			pRequest->Execute();
		}*/
	}
}