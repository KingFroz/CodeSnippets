#include "PathSearch.h"
#include <windows.h>

//#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 
#include <math.h>


namespace fullsail_ai { namespace algorithms {

	PathSearch::PathSearch() : openList(isGreater)
	{
		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		
		tileMap = NULL;
		found = false;
		visited = new unordered_map<Search*, Planner*>;
		Map = new unordered_map<Tile*, Search*>;
		destination = NULL;
	}

	PathSearch::~PathSearch()
	{
		shutdown();
		delete visited;
		delete Map;
	}

	void PathSearch::initialize(TileMap* _tileMap)
	{
		tileMap = _tileMap;
		for (int r = 0; r < tileMap->getRowCount(); ++r)
		{
			for (int c = 0; c < tileMap->getColumnCount(); ++c)
			{
				if (tileMap->getTile(r, c)->getWeight() > 0)
				{
					//Catching Node
					Search *tempNode = new Search(tileMap->getTile(r, c));

					//Set Search to tempNode, Planner to NULL
					pair<Tile*, Search*> setPair;
					setPair.first = tileMap->getTile(r, c);
					setPair.second = tempNode;
				
					Map->insert(setPair);
				}
			}
		}

		for (int r = 0; r < tileMap->getRowCount(); r++)
		{
			for (int c = 0; c < tileMap->getColumnCount(); c++)
			{
				if (tileMap->getTile(r, c)->getWeight() > 0) {
					//GOOD CHECKS
					Tile *Left = tileMap->getTile(r, (c - 1));
					Tile *Right = tileMap->getTile(r, (c + 1));
					Tile *Top = tileMap->getTile((r - 1), c);
					Tile *Bottom = tileMap->getTile((r + 1), c);

					//Even: GOOD CHECKS
					Tile *topRight = tileMap->getTile((r - 1), (c + 1));
					Tile *bottomRight = tileMap->getTile((r + 1), (c + 1));

					//Odd: GOOD CHECKS
					Tile *topLeft = tileMap->getTile((r - 1), (c - 1));
					Tile *bottomLeft = tileMap->getTile((r + 1), (c - 1));

					if (Left != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, Left)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(Left));
					}

					if (Right != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, Right)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(Right));
					}

					if (Top != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, Top)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(Top));
					}

					if (Bottom != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, Bottom)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(Bottom));
					}
					if (topRight != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, topRight)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(topRight));
					}

					if (topLeft != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, topLeft)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(topLeft));
					}

					if (bottomRight != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, bottomRight)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(bottomRight));
					}

					if (bottomLeft != NULL && Adjacent(Map->at(tileMap->getTile(r, c))->tileMap, bottomLeft)) {
						Map->at(tileMap->getTile(r, c))->Neighbors.push_back(Map->at(bottomLeft));
					}
				}

			}
		}
	}

	void PathSearch::enter(int startRow, int startColumn, int goalRow, int goalColumn)
	{
		found = false;
		
		while (!openList.empty())
		{
			openList.pop();
		}

		Search *start = Map->at(tileMap->getTile(startRow, startColumn));
		openList.push(new Planner(start));

		visited->clear();

		pair<Search*, Planner*> pairing;
		pairing.first = start;
		pairing.second = openList.front();

		visited->insert(pairing);

		//Set destination
		destination = tileMap->getTile(goalRow, goalColumn);
	}

	void PathSearch::update(long timeslice)
	{
		//Build solution list after solution is found
		//get tick count < timeslice
		//unsigned int blue = 0xFF0000FF;
		//unsigned int green = 0xFF00FF00;
		//unsigned int red = 0xFFFF0000;

		DWORD startTime = GetTickCount();
		DWORD elaspedTime = 0;
		
		while (!openList.empty())
		{
			Planner *current = openList.front();
			//current->vertex->tileMap->setFill(blue);
			if (current->givenCost == NULL)
			{
				current->givenCost = 0;
			}

			openList.pop();
			if (current->vertex->tileMap == destination)
			{
				solution.push_back(current->vertex->tileMap);

				auto iter = current;
				while (iter->parent != NULL)
				{
					iter = iter->parent;

					//iter->vertex->tileMap->setFill(red);
					solution.push_back(iter->vertex->tileMap);
				}

				found = true;
				return;
			}
	
			for (unsigned int i = 0; i < current->vertex->Neighbors.size(); ++i)
			{
				
				//current->vertex->Neighbors[i]->tileMap->setFill(blue);
				
				Search *successor = Map->at(current->vertex->Neighbors[i]->tileMap);
				float edgeCost = Distance(current->vertex, successor) * successor->tileMap->getWeight();
				float tempGivenCost = current->givenCost + edgeCost;
				if (visited->find(successor) != visited->end())
				{
					if (tempGivenCost < visited->at(successor)->givenCost)
					{
						Planner *successorNode = visited->at(successor);
						openList.remove(successorNode);
						successorNode->parent = current;
						successorNode->givenCost = tempGivenCost;
						openList.push(successorNode);
					}
				}
				else
				{
					Planner *successorNode = new Planner(successor);
					successorNode->parent = current;
					successorNode->givenCost = tempGivenCost;

					//Added to visited List
					pair<Search*, Planner*> pairing;
					pairing.first = successor;
					pairing.second = successorNode;

					visited->insert(pairing);

					openList.push(successorNode);
				}
			}

			elaspedTime = GetTickCount() - startTime;
			if ((long)elaspedTime >= timeslice)
			{
				break;
			}
		}
	}

	void PathSearch::exit()
	{
		//Clear open list: GOOD
		while (!openList.empty())
		{
			openList.pop();
		}
			
		//Clear Solution: GOOD
		solution.clear();

		//Clear visited list
		for (unordered_map<Search*, Planner*>::iterator iter = visited->begin(); iter != visited->end(); ++iter)
		{
			delete iter->second;
		}
		visited->clear();
	}

	void PathSearch::shutdown()
	{
		for (unordered_map<Tile*, Search*>::iterator iter = Map->begin(); iter != Map->end(); ++iter)
		{
			delete iter->second;
		}
		Map->clear();

		//Clear visited: GOOD

		for (unordered_map<Search*, Planner*>::iterator iter = visited->begin(); iter != visited->end(); ++iter)
		{
			delete iter->second;
		}
		visited->clear();
	}

	bool PathSearch::isDone() const
	{
		if (found)
			return true;

		return false;
	}

	std::vector<Tile const*> const PathSearch::getSolution() const
	{
		return solution;
	}

	//bool PathSearch::isGreater(Planner* lhs, Planner* rhs)
	//{
	//	return (lhs->heuristicCost > rhs->heuristicCost);
	//}

	float PathSearch::Distance(const Search *A, const Search *B)
	{
		float x = (float)(B->tileMap->getXCoordinate() - A->tileMap->getXCoordinate());
		float y = (float)(B->tileMap->getYCoordinate() - A->tileMap->getYCoordinate());

		float distance = x * x + y * y;
		return sqrt(distance);
	}

	bool PathSearch::Adjacent(const Tile *lhs, const Tile *rhs)
	{
		if(rhs->getRow() % 2)
		{
			//Right Neighbor: GOOD CHECK
			if (lhs->getRow() == rhs->getRow() && lhs->getColumn() + 1 == rhs->getColumn() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Left Neighbor: GOOD CHECK
			else if (lhs->getRow() == rhs->getRow() && lhs->getColumn() - 1 == rhs->getColumn() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Bottom Neighbor: GOOD CHECK
			else if (lhs->getColumn() == rhs->getColumn() && lhs->getRow() + 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Top Neighbor: GOOD CHECK
			else if (lhs->getColumn() == rhs->getColumn() && lhs->getRow() - 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}

			//Top Left Neighbor: GOOD CHECK
			else if (lhs->getColumn() - 1 == rhs->getColumn() && lhs->getRow() - 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Bottom Left Neighbor: GOOD CHECK
			else if (lhs->getColumn() - 1 == rhs->getColumn() && lhs->getRow() + 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}
		}
		else
		{
			//Right Neighbor: GOOD CHECK
			if (lhs->getRow() == rhs->getRow() && lhs->getColumn() + 1 == rhs->getColumn() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Left Neighbor: GOOD CHECK
			else if (lhs->getRow() == rhs->getRow() && lhs->getColumn() - 1 == rhs->getColumn() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Bottom Neighbor: GOOD CHECK
			else if (lhs->getColumn() == rhs->getColumn() && lhs->getRow() + 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Top Neighbor: GOOD CHECK
			else if (lhs->getColumn() == rhs->getColumn() && lhs->getRow() - 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Top Right Neighbor: GOOD CHECK
			else if (lhs->getColumn() + 1 == rhs->getColumn() && lhs->getRow() - 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}
			//Bottom Right Neighbor: GOOD CHECK
			else if (lhs->getColumn() + 1 == rhs->getColumn() && lhs->getRow() + 1 == rhs->getRow() && rhs->getWeight() != 0)
			{
				return true;
			}
		}

		return false;
	}

	bool isGreater(Planner* const& lhs, Planner* const& rhs)
	{
		return (lhs->givenCost > rhs->givenCost);
	}

}}  // namespace fullsail_ai::algorithms

