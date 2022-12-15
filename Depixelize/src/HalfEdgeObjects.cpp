#include "HalfEdgeObjects.h"

void HalfEdge::setVisibility()
{
	//visible = opp != NULL;
	//return;

	float colR1 = pixel->getColR();
	float colG1 = pixel->getColG();
	float colB1 = pixel->getColB();
	float colA1 = pixel->getColA();

	if (opp == NULL || opp->getPixel() == NULL)
	{
		visible = false;
		return;
	}
	PixelFace* pixel2 = opp->getPixel();

	float colR2 = pixel2->getColR();
	float colG2 = pixel2->getColG();
	float colB2 = pixel2->getColB();
	float colA2 = pixel2->getColA();

	// Here, an edge is visible if the colours of both its faces are different	
	visible = (colR1 != colR2) || (colG1 != colG2) || (colB1 != colB2) || (colA1 != colA2);
}

void HalfEdge::setVisibility(bool setVisible)
{
	visible = setVisible;
}

PixelFace::PixelFace()
{
	edge = NULL;
	colR = 0.0f;
	colG = 0.0f;
	colB = 0.0f;
	colA = 0.0f;
}

PixelFace::~PixelFace()
{}

VoroniVertex::~VoroniVertex()
{
	if (incomingOutgoingEdgePair != NULL)
		free(incomingOutgoingEdgePair);
}

void VoroniVertex::resolveTJunctions()
{
	std::vector<HalfEdge*> outgoingVisibleEdges;

	// Initialising above vector with outgoing visible edges
	HalfEdge* start = edge;
	HalfEdge* curr = start;
	do
	{
		if (curr == NULL)
			break;
		if (curr->isVisible())
		{
			outgoingVisibleEdges.push_back(curr);
		}

		// set curr to next edge around current vertex
		if (curr->getOpp() == NULL)
			break;
		curr = curr->getOpp()->getNext();
	} while (curr != start);

	// Path can only be set if at least 2 visible edges are incident on the edge
	if (outgoingVisibleEdges.size() >= 2)
	{
		// If 3 visible edges are incident, perform T-junction resolution
		if (outgoingVisibleEdges.size() == 3)
		{
			VoroniVertex* p1 = outgoingVisibleEdges[0]->getOpp()->getVertex();
			VoroniVertex* p2 = outgoingVisibleEdges[1]->getOpp()->getVertex();
			VoroniVertex* p3 = outgoingVisibleEdges[2]->getOpp()->getVertex();

			Vector2D v1(p1->getPosX() - posX, p1->getPosY() - posY);
			Vector2D v2(p2->getPosX() - posX, p2->getPosY() - posY);
			Vector2D v3(p3->getPosX() - posX, p3->getPosY() - posY);

			float v1v2 = v1 * v2;
			float v2v3 = v2 * v3;
			float v1v3 = v1 * v3;

			// If v2 and v3 have the least dot product, remove edge 1
			if (v2v3 <= v1v2 && v2v3 <= v1v3)
			{
				outgoingVisibleEdges.erase(outgoingVisibleEdges.begin());
			}
			// If v1 and v3 have the least dot product, remove edge 2
			else if (v1v3 <= v1v2 && v1v3 <= v2v3)
			{
				outgoingVisibleEdges.erase(outgoingVisibleEdges.begin() + 1);
			}
			// If v1 and v2 have the least dot product, remove edge 3
			else
			{
				outgoingVisibleEdges.erase(outgoingVisibleEdges.begin() + 2);
			}
		}
		// If 4 visible edges are incident, perform +-junction resolution
		if (outgoingVisibleEdges.size() == 4)
		{
			// We simply swap positions of the 2nd and 3rd edges, so each edge is paired with its opposite
			HalfEdge* temp = outgoingVisibleEdges[1];
			outgoingVisibleEdges[1] = outgoingVisibleEdges[2];
			outgoingVisibleEdges[2] = temp;
		}

		// Once edge resolutions performed, add edge values to incomingOutgoingEdgePair
		incomingOutgoingEdgePairCount = (outgoingVisibleEdges.size() / 2) * 2;
		if (incomingOutgoingEdgePair != NULL)
			free(incomingOutgoingEdgePair);
		incomingOutgoingEdgePair = (HalfEdge**)malloc(incomingOutgoingEdgePairCount * 2 * sizeof(HalfEdge*));
		int ptr = 0;

		for (int i = 0; i < incomingOutgoingEdgePairCount; i+=2)
		{
			HalfEdge* out1 = outgoingVisibleEdges[i];
			HalfEdge* out2 = outgoingVisibleEdges[i+1];
			HalfEdge* inc1 = out2->getOpp();
			HalfEdge* inc2 = out1->getOpp();

			incomingOutgoingEdgePair[ptr++] = inc1;
			incomingOutgoingEdgePair[ptr++] = out1;
			incomingOutgoingEdgePair[ptr++] = inc2;
			incomingOutgoingEdgePair[ptr++] = out2;
		}
	}
}

HalfEdge* VoroniVertex::getOutgoingOf(HalfEdge* incomingEdge)
{
	if (incomingOutgoingEdgePair == NULL)
		return NULL;
	for (int i = 0; i < incomingOutgoingEdgePairCount*2; i += 2)
	{
		if (incomingOutgoingEdgePair[i] == incomingEdge)
			return incomingOutgoingEdgePair[i + 1];
	}
	return NULL;
}

HalfEdge* VoroniVertex::getIncomingOf(HalfEdge* outgoingEdge)
{
	if (incomingOutgoingEdgePair == NULL)
		return NULL;
	for (int i = 1; i < incomingOutgoingEdgePairCount*2; i += 2)
	{
		if (incomingOutgoingEdgePair[i] == outgoingEdge)
			return incomingOutgoingEdgePair[i - 1];
	}
	return NULL;
}
