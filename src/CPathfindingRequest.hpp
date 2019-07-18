/* =========================================

		FCNPC - Fully Controllable NPC
			----------------------

	- File: CPathfindingRequest.hpp
	- Author(s): Freaksken

  =========================================*/

#ifndef PATHFINDINGREQUEST_H
#define PATHFINDINGREQUEST_H

class CPathfindingRequest
{
public:
	CPathfindingRequest(WORD wPlayerId, float fOriginX, float fOriginZ, float fOriginY, float fDestinationX, float fDestinationY, float fDestinationZ)
		: m_wPlayerId(wPlayerId), m_fOriginX(fOriginX), m_fOriginY(fOriginY), m_fOriginZ(fOriginZ), m_fDestinationX(fDestinationX), m_fDestinationY(fDestinationY), m_fDestinationZ(fDestinationZ) {};

	void Execute();

private:
	WORD m_wPlayerId;
	float m_fOriginX;
	float m_fOriginY;
	float m_fOriginZ;
	float m_fDestinationX;
	float m_fDestinationY;
	float m_fDestinationZ;
};

#endif
