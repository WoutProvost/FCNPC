/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: CPathfindingRequest.cpp
	- Author(s): Freaksken

  =========================================*/

#include "Main.hpp"

void CPathfindingRequest::Execute()
{
	logprintf("PATHFINDING: PLAYER %d, ORIGIN (%.2f,%.2f,%.2f), DESTINATION (%.2f,%.2f,%.2f)", m_wPlayerId, m_fOriginX, m_fOriginY, m_fOriginZ, m_fDestinationX, m_fDestinationY, m_fDestinationZ);
}