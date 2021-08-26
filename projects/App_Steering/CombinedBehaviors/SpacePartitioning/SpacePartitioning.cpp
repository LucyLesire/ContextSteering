#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\App_Steering\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = { left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
	, m_CellHeight{height*2/rows}
	, m_CellWidth{width*2/cols}
{
	m_Neighbors.reserve(maxEntities);
	//m_Cells.reserve(m_NrOfCols * m_NrOfRows);
	//for (int i{}; i < m_NrOfRows; i++)
	//{
	//	for (int j{}; j < m_NrOfCols; j++)
	//	{
	//		m_Cells[i ].boundingBox = Elite::Rect{ Elite::Vector2{i * m_SpaceHeight / m_NrOfRows, j * m_SpaceWidth / m_NrOfCols}, m_CellHeight, m_CellWidth };
	//	}
	//}

	float bottom{ -width };
	float left{ -height };
	const float cwidth{ (width * 2) / m_NrOfRows };
	const float cheight{ (height * 2) / m_NrOfCols };


	for (int i = 0; i < m_NrOfCols; i++)
	{
		for (int j = 0; j < m_NrOfRows; j++)
		{
			m_Cells.push_back({ left ,bottom,cwidth, cheight });
			//left += (m_SpaceWidth / m_NrOfRows) * 2;
			left += cwidth;
		}
		left = -m_SpaceWidth;
		//bottom += (m_SpaceHeight / m_NrOfCols) * 2;
		bottom += cheight;
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::UpdateAgentCell(SteeringAgent* agent, const Elite::Vector2& oldPos)
{
	Elite::Vector2 pos{ agent->GetPosition() };
	int index{ PositionToIndex(pos) };
	int oldIndex{ PositionToIndex(oldPos) };
	if (!(index == oldIndex))
	{
		AddAgent(agent);
		m_Cells[PositionToIndex(oldPos)].agents.remove(agent);
	}
}

void CellSpace::RegisterNeighbors(const Elite::Vector2& pos, float queryRadius)
{
	float distance{ };
	m_NrOfNeighbors = 0;
	Elite::Rect neighbourRect{ {pos.x - queryRadius, pos.y - queryRadius}, queryRadius * 2, queryRadius * 2 };
	for (int i{}; i < m_Cells.size(); ++i)
	{
		if (Elite::IsOverlapping(neighbourRect, m_Cells[i].boundingBox))
		{
			for (SteeringAgent* agent : m_Cells[i].agents)
			{
				distance = Elite::Distance(agent->GetPosition(), pos);
				if (distance < queryRadius)
				{
					m_Neighbors[m_NrOfNeighbors] = agent;
					m_NrOfNeighbors++;
				}
			}
		}
	}
}

void CellSpace::RenderCells(bool canRender, Elite::Vector2 posAgent0, float queryRadius) const
{
	if (canRender)
	{
		for (const Cell& c : m_Cells)
		{
			Elite::Polygon cellPoly{ c.GetRectPoints() };
			Elite::Rect neighbourRect{ {posAgent0.x - queryRadius, posAgent0.y - queryRadius}, queryRadius * 2, queryRadius * 2 };
			Elite::Color cellColor{};
			if (Elite::IsOverlapping(neighbourRect, c.boundingBox))
			{
				cellColor = { 1.f,0.f,0.f };
			}
			else
			{
				cellColor = { 1.f,1.f,1.f };
			}
			DEBUGRENDERER2D->DrawPolygon(&cellPoly, cellColor);
			DEBUGRENDERER2D->DrawString(c.GetRectPoints()[1], std::to_string(c.agents.size()).c_str());
		}
	}

}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	Elite::Vector2 position{pos};
	if (pos.x > maxRange)
	{
		position.x = maxRange;
	}
	else if (pos.x < -maxRange)
	{
		position.x = -maxRange;
	}
	if (pos.y > maxRange)
	{
		position.y = maxRange;
	}
	else if (pos.y < -maxRange)
	{
		position.y = -maxRange;
	}
	for (int i{}; m_Cells.size(); ++i)
	{
		if (Elite::IsOverlapping(m_Cells[i].boundingBox, Elite::Rect{ position,1,1 }))
		{
			return i;
		}
	}
	return 0;
}