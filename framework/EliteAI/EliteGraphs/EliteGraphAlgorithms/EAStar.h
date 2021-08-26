#pragma once

namespace Elite
{
	template <class T_NodeType, class T_ConnectionType>
	class AStar
	{
	public:
		AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			T_NodeType* pNode = nullptr;
			T_ConnectionType* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<T_NodeType*> FindPath(T_NodeType* pStartNode, T_NodeType* pDestinationNode);

	private:
		float GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const;

		IGraph<T_NodeType, T_ConnectionType>* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	template <class T_NodeType, class T_ConnectionType>
	AStar<T_NodeType, T_ConnectionType>::AStar(IGraph<T_NodeType, T_ConnectionType>* pGraph, Heuristic hFunction)
		: m_pGraph(pGraph)
		, m_HeuristicFunction(hFunction)
	{
	}

	template <class T_NodeType, class T_ConnectionType>
	std::vector<T_NodeType*> AStar<T_NodeType, T_ConnectionType>::FindPath(T_NodeType* pStartNode, T_NodeType* pGoalNode)
	{
		vector<T_NodeType*> path;
		vector<NodeRecord> openList;
		vector<NodeRecord> closedList;
		NodeRecord currentRecord;

		currentRecord.pNode = pStartNode;
		currentRecord.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode);
		openList.push_back(currentRecord);

		while (!openList.empty())
		{
			currentRecord = *std::min_element(openList.begin(), openList.end());
			if (currentRecord.pNode == pGoalNode && currentRecord.pNode != nullptr)
			{
				break;
			}
			for (auto con : m_pGraph->GetNodeConnections(currentRecord.pNode->GetIndex()))
			{
				NodeRecord connectionRecord;
				connectionRecord.pNode = m_pGraph->GetNode(con->GetTo());
				connectionRecord.costSoFar = con->GetCost() + currentRecord.costSoFar;

				T_NodeType* nextNode{ m_pGraph->GetNode(con->GetTo()) };
				auto IsNodeInList = [nextNode](const NodeRecord& nodeRec){return nodeRec.pNode == nextNode; };
				auto checkFind = std::find_if(closedList.begin(), closedList.end(), IsNodeInList);

				if (checkFind != closedList.end())
				{
					if (checkFind->costSoFar < connectionRecord.costSoFar)
					{
						continue;
					}
					else
					{
						closedList.erase(checkFind);
					}
				}
				else
				{
					checkFind = std::find_if(openList.begin(), openList.end(), IsNodeInList);
					if (checkFind != openList.end())
					{
						if (checkFind->costSoFar < connectionRecord.costSoFar)
						{
							continue;
						}
						else
						{
							openList.erase(checkFind);
						}
					}
				}
				connectionRecord.pConnection = con;
				connectionRecord.estimatedTotalCost = connectionRecord.costSoFar + GetHeuristicCost(nextNode, pGoalNode);
				openList.push_back(connectionRecord);
			}
			openList.erase(std::remove(openList.begin(), openList.end(), currentRecord));
			closedList.push_back(currentRecord);
		}

		//We reached the goal node
		//Start backtracking

		while (currentRecord.pNode != pStartNode)
		{
			path.push_back(currentRecord.pNode);
			for (const NodeRecord& nWordRecord : closedList)
			{
				if (nWordRecord.pNode != nullptr && nWordRecord.pNode->GetIndex() == currentRecord.pConnection->GetFrom())
				{
					currentRecord = nWordRecord;
					break;
				}
			}
		}

		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());

		return path;
	}

	template <class T_NodeType, class T_ConnectionType>
	float Elite::AStar<T_NodeType, T_ConnectionType>::GetHeuristicCost(T_NodeType* pStartNode, T_NodeType* pEndNode) const
	{
		Vector2 toDestination = m_pGraph->GetNodePos(pEndNode) - m_pGraph->GetNodePos(pStartNode);
		return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
	}
}