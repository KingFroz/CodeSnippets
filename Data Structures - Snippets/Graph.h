#pragma once
#include "DynArray.h"
#include "SLList.h"
#include <iostream>
#include <list>
#include <queue>

using namespace std;

template<typename Type>
class Graph
{
public:
	struct Edge
	{
		unsigned int toVertex;
	};

	struct Vertex
	{
		// the data that this vertex is storing
		Type element;

		// the list of edges that connect this vertex to another vertex
		SLList<Edge> edges;

		///////////////////////////////////////////////////////////////////////////
		// Function : addEdge
		// Parameters : toVertex - the index of the vertex we are adjacent to
		///////////////////////////////////////////////////////////////////////////
		void addEdge(const unsigned int& toVertex)
		{
			Edge connect;
			connect.toVertex = toVertex;
			edges.addHead(connect);
		}
	};

private:
	DynArray<Vertex> Vertices;
public:
	unsigned int addVertex(const Type& value);
	Vertex& operator[](const unsigned int& index);
	unsigned int size() const;
	void clear();
	void printBreadthFirst(const unsigned int& startVertex);
};

/////////////////////////////////////////////////////////////////////////////
// Function : addVertex
// Parameters : value - the data to store in this vertex
// Return : unsigned int - the index this vertex was added at
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
unsigned int Graph<Type>::addVertex(const Type& value)
{
	Vertex vertex;
	vertex.element = value;

	Vertices.append(vertex);

	return Vertices.size() - 1;
}
/////////////////////////////////////////////////////////////////////////////
// Function : operator[]
// Parameters : index - the index in the graph to access
// Return : Vertex& - the vertex stored at the specified index
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
typename Graph<Type>::Vertex& Graph<Type>::operator[](const unsigned int& index)
{
	return Vertices[index];
}
/////////////////////////////////////////////////////////////////////////////
// Function : size
// Return : unsiged int - the number of vertices in the graph
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
unsigned int Graph<Type>::size() const
{
	return Vertices.size();
}
/////////////////////////////////////////////////////////////////////////////
// Function : clear
// Notes : clears the graph and readies it for re-use
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void Graph<Type>::clear()
{
	Vertices.clear();
}
/////////////////////////////////////////////////////////////////////////////
// Function : printBreadthFirst
// Parameters : startVertex - the vertex to start the traversal at
// Notes : prints contents of the graph in depth order 
//			(from closest to farthest)
/////////////////////////////////////////////////////////////////////////////
template<typename Type>
void Graph<Type>::printBreadthFirst(const unsigned int& startVertex)
{
	DynArray<int> Graph;
	queue<unsigned int> print;

	//Copy Vertexes DynArray --

	Graph.reserve(Vertices.size());
	unsigned int i = 0;
	for (; i < Vertices.size(); i++)
	{
		Graph.append(-1);
	}

	//How do I check for edges?
	print.push(startVertex);
	Graph[startVertex] = 0;

	int index = 0;

	while (!print.empty())
	{		
		index = print.front();
		print.pop();

		SLLIter<Edge> iter(Vertices[index].edges);
		iter.begin();

		for (; !iter.end();)
		{
			
			if (Graph[iter.current().toVertex] < 0)
			{
				print.push(iter.current().toVertex);
				Graph[iter.current().toVertex] = Graph[index] + 1;
			}

			++iter;
		}

		cout << Vertices[index].element << " : " << Graph[index] << endl;
	}
}