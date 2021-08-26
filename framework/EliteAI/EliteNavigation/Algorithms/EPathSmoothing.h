#pragma once

#include <vector>
#include "framework/EliteGeometry/EGeometry2DTypes.h"
#include "framework/EliteAI/EliteGraphs/EGraphNodeTypes.h"

namespace Elite
{
	//Portal struct (only contains line info atm, you can expand this if needed)
	struct Portal
	{
		Portal() {}
		explicit Portal(const Elite::Line& line) :
			Line(line)
		{}
		Elite::Line Line = {};
	};


	class SSFA final
	{
	public:
		//=== SSFA Functions ===
		//--- References ---
		//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
		//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
		static std::vector<Portal> FindPortals(
			const std::vector<NavGraphNode*>& nodePath,
			Polygon* navMeshPolygon)
		{
			//Container
			std::vector<Portal> vPortals = {};

			vPortals.push_back(Portal(Line(nodePath[0]->GetPosition(), nodePath[0]->GetPosition())));

			//For each node received, get it's corresponding line
			for (size_t i = 1; i < nodePath.size() - 1; ++i)
			{
				//Local variables
				auto pNode = nodePath[i]; //Store node, except last node, because this is our target node!
				auto pLine = navMeshPolygon->GetLines()[pNode->GetLineIndex()];

				//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
				auto centerLine = (pLine->p1 + pLine->p2) / 2.0f;
				auto previousPosition = i == 0 ? nodePath[0]->GetPosition() : nodePath[i - 1]->GetPosition();
				auto cp = Cross((centerLine - previousPosition), (pLine->p1 - previousPosition));
				Line portalLine = {};
				if (cp > 0)//Left
					portalLine = Line(pLine->p2, pLine->p1);
				else //Right
					portalLine = Line(pLine->p1, pLine->p2);

				//Store portal
				vPortals.push_back(Portal(portalLine));
			}
			//Add degenerate portal to force end evaluation
			vPortals.push_back(Portal(Line(nodePath[nodePath.size()-1]->GetPosition(), nodePath[nodePath.size() - 1]->GetPosition())));

			return vPortals;
		}

		static std::vector<Elite::Vector2> OptimizePortals(const std::vector<Portal>& portals)
		{
			//P1 == right point of portal, P2 == left point of portal
			std::vector<Elite::Vector2> vPath = {};
			auto apex = portals[0].Line.p1;
			auto apexIndex = 0, leftLegIndex = 1, rightLegIndex = 1;
			auto rightLeg = portals[rightLegIndex].Line.p2 - apex;
			auto leftLeg = portals[leftLegIndex].Line.p1 - apex;

			for (unsigned int i = 1; i < static_cast<unsigned int>(portals.size()); ++i)
			{
				//Local
				const auto &portal = portals[i];

				//--- RIGHT CHECK ---
				Vector2 rightLeg2 = portals[i].Line.p2 - apex;
				//1. See if moving funnel inwards - RIGHT
				if (Cross(rightLeg2.GetNormalized(), rightLeg.GetNormalized()) >= 0.f)
				{
					//2. See if new line degenerates a line segment - RIGHT
					if (Cross(leftLeg.GetNormalized(), rightLeg2.GetNormalized()) < 0.f)
					{
						apex += leftLeg;
						apexIndex = leftLegIndex;
						int newIterator{ leftLegIndex + 1 };
						i = newIterator;
						leftLegIndex = newIterator;
						rightLegIndex = newIterator;
						vPath.push_back(apex);

						if (newIterator < static_cast<unsigned int>(portals.size()))
						{
							leftLeg = portals[rightLegIndex].Line.p1 - apex;
							rightLeg = portals[leftLegIndex].Line.p2 - apex;
							continue;
						}
					}
					else
					{
						rightLeg = rightLeg2;
						rightLegIndex = i;
					}
				}

				//--- LEFT CHECK ---
				Vector2 leftLeg2 = portals[i].Line.p1 - apex;
				//1. See if moving funnel inwards - LEFT
				if (Cross(leftLeg.GetNormalized(), leftLeg2.GetNormalized()) >= 0)
				{
					//2. See if new line degenerates a line segment - LEFT
					if (Cross(leftLeg2.GetNormalized(), rightLeg.GetNormalized()) < 0.f)
					{
						apex += rightLeg;
						apexIndex = rightLegIndex;
						int newIterator{ rightLegIndex + 1 };
						i = newIterator;
						rightLegIndex = newIterator;
						leftLegIndex = newIterator;
						vPath.push_back(apex);

						if (newIterator < static_cast<unsigned int>(portals.size()))
						{
							leftLeg = portals[rightLegIndex].Line.p1 - apex;
							rightLeg = portals[leftLegIndex].Line.p2 - apex;
							continue;
						}
					}
					else
					{
						leftLeg = leftLeg2;
						leftLegIndex = i;
					}
				}
			}
			
			// Add last path point (You can use the last portal p1 or p2 points as both are equal to the endPoint of the path)
			vPath.push_back(portals.back().Line.p1);

			return vPath;
		}
	private:
		SSFA() {};
		~SSFA() {};
	};
}
