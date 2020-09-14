#pragma once
class PajekHelper
{
};
#pragma once
#include <string>

class Pajek
{
	Pajek(Vertices vt) {

	}
};

class Vertices
{
	Vertices(size_t n) {
		Node* nodeArray = new Node[n];
	}
};

class Node
{
	int nodeid;
	Label label;
	int x;
	int y;


public:
	Node() {}
	Node(int nodeid, Label label) :
		nodeid(nodeid),
		label(label)

	{

	}
};

class Label {

	std::string label;

public:

	Label(std::string lavel) :label(label) {

	}
	
};


class Edges
{

};


