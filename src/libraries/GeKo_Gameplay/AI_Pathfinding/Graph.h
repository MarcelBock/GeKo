#pragma once
#include <GeKo_Gameplay/AI_Pathfinding/GraphNode.h>
#include <GeKo_Gameplay/AI_Pathfinding/Algorithm.h>
#include <vector>

/** The Graph-class provides a "bucket" for GraphNodes, which cann be added to the vector-list ot the Graph. A Graph represents one closed path-system on which a AI-Unit can run.
For example one Graph could represent the search for food or the area, which the AI should protect.*/

template<class T, class A>

class Graph
{
public: 
	Graph(){}
	~Graph(){}
	
	///Adds a GraphNode-Object to the vector-list m_nodes
	/**/
	 void addGraphNode(T* node)
	 {
		 m_nodes.push_back(node);
	 }
	 
	 ///Returns the vector-list m_nodes completely
	 /**/
	 std::vector<T*>* getGraph()
	 {
		 return &m_nodes;
	 }

	 void setAlgorithm(A* algorithm)
	 {
		 m_algorithm = algorithm;
	 }

	 A* getAlgorithm()
	 {
		 return m_algorithm;
	 }
	
protected:
	  std::vector<T*> m_nodes;

	  A* m_algorithm;
};