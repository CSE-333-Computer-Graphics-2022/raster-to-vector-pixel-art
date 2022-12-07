#include "Voronigraph.h"

VoroniGraph::VoroniGraph(RasterSprite* rasterSprite, SimilarityGraph* similarityGraph)
	:sprite(rasterSprite), graph(similarityGraph)
{
	setPixelFaces();
	setSubPixels();
	constructHalfEdges();
	setVoroniGraphTriangles();
}

VoroniGraph::~VoroniGraph()
{
	int pixelRows = sprite->getPixelRows();
	int pixelCols = sprite->getPixelCols();
	for (int i = 0; i < pixelRows - 1; i++)
	{
		free(subpixels[i]);
	}
	free(subpixels);
	for (int i = 0; i < pixelRows; i++)
	{
		free(pixelFaces[i]);
	}
	free(pixelFaces);
	free(voroniGraphTriangles);
	free(voroniGraphIndices);
}

void VoroniGraph::setPixelFaces()
{
	int pixelRows = sprite->getPixelRows();
	int pixelCols = sprite->getPixelCols();
	//float* graphVertexPositions = graph->getGraphVertexVertices();
	std::vector<std::vector<unsigned int>> pixelData = sprite->getPixelData();

	pixelFaces = (PixelFace**)malloc(pixelRows * sizeof(PixelFace*));
	for (int i = 0; i < pixelRows; i++)
	{
		pixelFaces[i] = (PixelFace*)malloc(pixelCols * sizeof(PixelFace));
		for (int j = 0; j < pixelCols; j++)
		{
			pixelFaces[i][j] = PixelFace();
		}
	}

	int ptr = 0;
	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			float x = (2 * j - (pixelCols - 1)) * sprite->getPixelRenderWidth() / 2;
			float y = (2 * (pixelRows - i - 1) - (pixelRows - 1)) * sprite->getPixelRenderHeight() / 2;

			pixelFaces[i][j].setPosition(x,y);
			//std::cout << i << "  " << j << " | " << x << " " << y << std::endl;

			float colR = ((float)pixelData[i][4 * j + 0]) / 255.0f;
			float colG = ((float)pixelData[i][4 * j + 1]) / 255.0f;
			float colB = ((float)pixelData[i][4 * j + 2]) / 255.0f;
			float colA = ((float)pixelData[i][4 * j + 3]) / 255.0f;

			pixelFaces[i][j].setColor(colR, colG, colB, colA);
		}
	}
}

void VoroniGraph::setSubPixels()
{
	std::cout << "SETTING SUBPIXELS" << std::endl;
	int pixelRows = sprite->getPixelRows();
	int pixelCols = sprite->getPixelCols();

	subpixels = (SubPixel**)malloc((pixelRows - 1) * sizeof(SubPixel*));
	for (int i = 0; i < pixelRows - 1; i++)
	{
		subpixels[i] = (SubPixel*)malloc((pixelCols - 1) * sizeof(SubPixel));
		for (int j = 0; j < pixelCols - 1; j++)
		{
			subpixels[i][j] = SubPixel();
		}
	}

	// Set the top left subpixel
	VoroniVertex *v = new VoroniVertex((pixelFaces[0][0].getPosX() + pixelFaces[0][1].getPosX()) / 2,
									   (pixelFaces[0][0].getPosY() + pixelFaces[0][1].getPosY()) / 2);
	subpixels[0][0].setVertexU(v);
	v = new VoroniVertex((pixelFaces[1][0].getPosX() + pixelFaces[1][1].getPosX()) / 2,
						 (pixelFaces[1][0].getPosY() + pixelFaces[1][1].getPosY()) / 2);
	subpixels[0][0].setVertexD(v);
	v = new VoroniVertex((pixelFaces[0][0].getPosX() + pixelFaces[1][0].getPosX()) / 2,
						 (pixelFaces[0][0].getPosY() + pixelFaces[1][0].getPosY()) / 2);
	subpixels[0][0].setVertexL(v);
	v = new VoroniVertex((pixelFaces[0][1].getPosX() + pixelFaces[1][1].getPosX()) / 2,
						 (pixelFaces[0][1].getPosY() + pixelFaces[1][1].getPosY()) / 2);
	subpixels[0][0].setVertexR(v);

	// Set the top row
	for (int i = 1; i < pixelCols - 1; i++)
	{
		subpixels[0][i].setVertexL(subpixels[0][i - 1].getVertexR());
		v = new VoroniVertex((pixelFaces[0][i].getPosX() + pixelFaces[0][i+1].getPosX()) / 2,
						     (pixelFaces[0][i].getPosY() + pixelFaces[0][i+1].getPosY()) / 2);
		subpixels[0][i].setVertexU(v);
		v = new VoroniVertex((pixelFaces[1][i].getPosX() + pixelFaces[1][i + 1].getPosX()) / 2,
							 (pixelFaces[1][i].getPosY() + pixelFaces[1][i + 1].getPosY()) / 2);
		subpixels[0][i].setVertexD(v);
		v = new VoroniVertex((pixelFaces[0][i+1].getPosX() + pixelFaces[1][i+1].getPosX()) / 2,
							 (pixelFaces[0][i+1].getPosY() + pixelFaces[1][i+1].getPosY()) / 2);
		subpixels[0][i].setVertexR(v);
	}

	// Set the left column
	for (int i = 1; i < pixelRows - 1; i++)
	{
		subpixels[i][0].setVertexU(subpixels[i - 1][0].getVertexD());
		v = new VoroniVertex((pixelFaces[i+1][0].getPosX() + pixelFaces[i+1][1].getPosX()) / 2,
							 (pixelFaces[i+1][0].getPosY() + pixelFaces[i+1][1].getPosY()) / 2);
		subpixels[i][0].setVertexD(v);
		v = new VoroniVertex((pixelFaces[i][0].getPosX() + pixelFaces[i+1][0].getPosX()) / 2,
							 (pixelFaces[i][0].getPosY() + pixelFaces[i+1][0].getPosY()) / 2);
		subpixels[i][0].setVertexL(v);
		v = new VoroniVertex((pixelFaces[i][1].getPosX() + pixelFaces[i+1][1].getPosX()) / 2,
							 (pixelFaces[i][1].getPosY() + pixelFaces[i+1][1].getPosY()) / 2);
		subpixels[i][0].setVertexR(v);
	}

	// Set all other subpixels
	for (int i = 1; i < pixelRows - 1; i++)
	{
		for (int j = 1; j < pixelCols - 1; j++)
		{
			subpixels[i][j].setVertexU(subpixels[i - 1][j].getVertexD());
			subpixels[i][j].setVertexL(subpixels[i][j - 1].getVertexR());
			v = new VoroniVertex((pixelFaces[i + 1][j].getPosX() + pixelFaces[i + 1][j+1].getPosX()) / 2,
								 (pixelFaces[i + 1][j].getPosY() + pixelFaces[i + 1][j+1].getPosY()) / 2);
			subpixels[i][j].setVertexD(v);
			v = new VoroniVertex((pixelFaces[i][j+1].getPosX() + pixelFaces[i + 1][j+1].getPosX()) / 2,
								 (pixelFaces[i][j+1].getPosY() + pixelFaces[i + 1][j+1].getPosY()) / 2);
			subpixels[i][j].setVertexR(v);
		}
	}

	std::cout << "SUBPIXELS SET" << std::endl;
}

void VoroniGraph::constructHalfEdges()
{
	constructBorderEdges();
	//constructSubPixelEdges();
}

void VoroniGraph::setVoroniGraphTriangles()
{
	std::vector<float> renderVertices;

	int pixelRows = sprite->getPixelRows();
	int pixelCols = sprite->getPixelCols();

	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			//std::cout << "CYCLING " << i << " " << j << " | " << pixelFaces[i][j].getPosX() << " " << pixelFaces[i][j].getPosY() << std::endl;
			HalfEdge* start = pixelFaces[i][j].getEdge();
			HalfEdge* curr = start;
			int rep = 0;
			do
			{
				if (curr == NULL || curr->getNext() == NULL)
					break;

				renderVertices.push_back(pixelFaces[i][j].getPosX());
				renderVertices.push_back(pixelFaces[i][j].getPosY());
				renderVertices.push_back(pixelFaces[i][j].getColR());
				renderVertices.push_back(pixelFaces[i][j].getColG());
				renderVertices.push_back(pixelFaces[i][j].getColB());
				renderVertices.push_back(pixelFaces[i][j].getColA());

				renderVertices.push_back(curr->getVertex()->getPosX());
				renderVertices.push_back(curr->getVertex()->getPosY());
				renderVertices.push_back(pixelFaces[i][j].getColR());
				renderVertices.push_back(pixelFaces[i][j].getColG());
				renderVertices.push_back(pixelFaces[i][j].getColB());
				renderVertices.push_back(pixelFaces[i][j].getColA());

				//std::cout << curr->getVertex()->getPosX() << " " << curr->getVertex()->getPosY() << std::endl;

				curr = curr->getNext();
				if (curr == start)
					rep++;

				renderVertices.push_back(curr->getVertex()->getPosX());
				renderVertices.push_back(curr->getVertex()->getPosY());
				renderVertices.push_back(pixelFaces[i][j].getColR());
				renderVertices.push_back(pixelFaces[i][j].getColG());
				renderVertices.push_back(pixelFaces[i][j].getColB());
				renderVertices.push_back(pixelFaces[i][j].getColA());

			} while (rep < 1);
		}
	}
	
	// Transfer all data from the vector to the array
	voroniGraphTriangleCount = renderVertices.size();
	voroniGraphTriangles = (float*)malloc(voroniGraphTriangleCount * sizeof(float));
	voroniGraphIndices = (unsigned int*)malloc(voroniGraphTriangleCount * (2 + 4) * sizeof(unsigned int));
	int ptr = 0;
	for (float a : renderVertices)
	{
		voroniGraphTriangles[ptr++] = a;
	}
	ptr = 0;
	while (ptr < voroniGraphTriangleCount)
	{
		voroniGraphIndices[ptr++] = ptr;
	}
}

void VoroniGraph::simplifyVoroniGraph()
{
	int pixelRows = sprite->getPixelRows();
	int pixelCols = sprite->getPixelCols();

	for (int i = 1; i < pixelRows - 2; i++)
	{
		for (int j = 1; j < pixelCols - 2; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				VoroniVertex* v = subpixels[i][j].getVertexU();
				if (k == 0) v = subpixels[i][j].getVertexU();
				if (k == 1) v = subpixels[i][j].getVertexD();
				if (k == 2) v = subpixels[i][j].getVertexL();
				if (k == 3) v = subpixels[i][j].getVertexR();

				v->getEdge()->getOpp()->setNextwithPrev(v->getEdge()->getPrev()->getOpp()->getNext());
				v->getEdge()->getPrev()->setNextwithPrev(v->getEdge()->getNext());
				
			}
		}
	}
}

void VoroniGraph::constructBorderEdges()
{
	std::cout << "STARTING MAKING EDGES" << std::endl;
	int pixelRows = sprite->getPixelRows();
	int pixelCols = sprite->getPixelCols();

	// These will form the border vertices of the vectorised iamge
	VoroniVertex* topRowVertices = (VoroniVertex*)malloc((pixelCols - 1)*sizeof(VoroniVertex));
	VoroniVertex* bottomRowVertices = (VoroniVertex*)malloc((pixelCols - 1) * sizeof(VoroniVertex));
	VoroniVertex* leftColVertices = (VoroniVertex*)malloc((pixelRows - 1) * sizeof(VoroniVertex));
	VoroniVertex* rightColVertices = (VoroniVertex*)malloc((pixelRows - 1) * sizeof(VoroniVertex));

	// Initialising coordinates of the border (open edge) vertices
	for (int i = 0; i < pixelCols - 1; i++)
	{
		topRowVertices[i] = VoroniVertex((pixelFaces[0][i].getPosX() + pixelFaces[0][i + 1].getPosX()) / 2,
			pixelFaces[0][i].getPosY() + sprite->getPixelRenderHeight() / 2);
		bottomRowVertices[i] = VoroniVertex((pixelFaces[0][i].getPosX() + pixelFaces[0][i + 1].getPosX()) / 2,
			pixelFaces[pixelRows-1][i].getPosY() - sprite->getPixelRenderHeight() / 2);
		std::cout << topRowVertices[i].getPosX() << " ";
	}
	std::cout << std::endl;
	for (int i = 0; i < pixelRows - 1; i++)
	{
		leftColVertices[i] = VoroniVertex(pixelFaces[i][0].getPosX() - sprite->getPixelRenderWidth()/2,
			(pixelFaces[i][0].getPosY() + pixelFaces[i + 1][0].getPosY()) / 2);
		rightColVertices[i] = VoroniVertex(pixelFaces[i][pixelCols-1].getPosX() + sprite->getPixelRenderWidth() / 2,
			(pixelFaces[i][0].getPosY() + pixelFaces[i + 1][0].getPosY()) / 2);
	}

	// These will form the CORNER vertices of the vectorised image
	VoroniVertex vertexTL(pixelFaces[0][0].getPosX() - sprite->getPixelRenderWidth() / 2, pixelFaces[0][0].getPosY() + sprite->getPixelRenderHeight() / 2);
	VoroniVertex vertexTR(pixelFaces[0][pixelCols-1].getPosX() + sprite->getPixelRenderWidth() / 2, pixelFaces[0][pixelCols-1].getPosY() - sprite->getPixelRenderHeight() / 2);
	VoroniVertex vertexBL(pixelFaces[pixelRows-1][0].getPosX() - sprite->getPixelRenderWidth() / 2, pixelFaces[pixelRows-1][0].getPosY() + sprite->getPixelRenderHeight() / 2);
	VoroniVertex vertexBR(pixelFaces[pixelRows-1][pixelCols - 1].getPosX() + sprite->getPixelRenderWidth() / 2, pixelFaces[pixelRows-1][pixelCols - 1].getPosY() - sprite->getPixelRenderHeight() / 2);


	// Constructing edges for the TOP LEFT Corner
	topRowVertices[0].setEdge(new HalfEdge(&topRowVertices[0], &pixelFaces[0][0]));
	vertexTL.setEdge(new HalfEdge(&vertexTL, &pixelFaces[0][0]));
	topRowVertices[0].getEdge()->setNextwithPrev(vertexTL.getEdge());

	// Constructing edges for TOP ROW
	for (int i = 0; i < pixelCols - 1; i++)
	{
		subpixels[0][i].getVertexU()->setEdge(new HalfEdge(subpixels[0][i].getVertexU(), &pixelFaces[0][i]));
		subpixels[0][i].getVertexU()->getEdge()->setNextwithPrev(topRowVertices[i].getEdge());

		HalfEdge* p = new HalfEdge(&topRowVertices[i], &pixelFaces[0][i + 1]);
		VoroniVertex* nextVertex = &vertexTR;
		if (i + 1 < pixelCols - 1)
			nextVertex = &topRowVertices[i + 1];

		nextVertex->setEdge(new HalfEdge(nextVertex, &pixelFaces[0][i + 1]));
		nextVertex->getEdge()->setNextwithPrev(p);

		subpixels[0][i].getVertexU()->getEdge()->setOppPair(p);
	}

	// Constructing edges for RIGHT COLUMN
	rightColVertices[0].setEdge(new HalfEdge(&rightColVertices[0], &pixelFaces[0][pixelCols - 1]));
	rightColVertices[0].getEdge()->setNextwithPrev(vertexTR.getEdge());
	for (int i = 0; i < pixelRows - 1; i++)
	{
		HalfEdge* e = new HalfEdge(subpixels[i][pixelCols - 2].getVertexR(), &pixelFaces[i][pixelCols - 1]);
		subpixels[i][pixelCols - 2].getVertexR()->setEdge(e);
		subpixels[i][pixelCols - 2].getVertexR()->getEdge()->setNextwithPrev(rightColVertices[i].getEdge());

		HalfEdge* p = new HalfEdge(&rightColVertices[i], &pixelFaces[i + 1][pixelCols - 1]);
		VoroniVertex* nextVertex = &vertexBR;
		if (i + 1 < pixelRows - 1)
			nextVertex = &rightColVertices[i + 1];

		nextVertex->setEdge(new HalfEdge(nextVertex, &pixelFaces[i + 1][pixelCols - 1]));
		nextVertex->getEdge()->setNextwithPrev(p);

		subpixels[i][pixelCols - 2].getVertexR()->getEdge()->setOppPair(p);
	}

	// Constructing edges for BOTTOM ROW
	bottomRowVertices[pixelCols - 2].setEdge(new HalfEdge(&bottomRowVertices[pixelCols - 2], &pixelFaces[pixelRows - 1][pixelCols - 1]));
	bottomRowVertices[pixelCols - 2].getEdge()->setNextwithPrev(vertexBR.getEdge());
	for (int i = pixelCols - 2; i >= 0; i--)
	{
		subpixels[pixelRows - 2][i].getVertexD()->setEdge(new HalfEdge(subpixels[pixelRows - 2][i].getVertexD(), &pixelFaces[pixelRows - 1][i + 1]));
		subpixels[pixelRows - 2][i].getVertexD()->getEdge()->setNextwithPrev(bottomRowVertices[i].getEdge());

		HalfEdge* p = new HalfEdge(&bottomRowVertices[i], &pixelFaces[pixelRows - 1][i]);
		VoroniVertex* nextVertex = &vertexBL;
		if (i > 0)
			nextVertex = &bottomRowVertices[i - 1];

		nextVertex->setEdge(new HalfEdge(nextVertex, &pixelFaces[pixelRows - 1][i]));
		nextVertex->getEdge()->setNextwithPrev(p);

		subpixels[pixelRows - 2][i].getVertexD()->getEdge()->setOppPair(p);
	}
	
	// Constructing edges for LEFT COLUMN
	leftColVertices[pixelRows - 2].setEdge(new HalfEdge(&leftColVertices[pixelRows - 2], &pixelFaces[pixelRows - 1][0]));
	leftColVertices[pixelRows - 2].getEdge()->setNextwithPrev(vertexBL.getEdge());
	for (int i = pixelRows - 2; i >= 0; i--)
	{
		subpixels[i][0].getVertexL()->setEdge(new HalfEdge(subpixels[i][0].getVertexL(), &pixelFaces[i + 1][0]));
		subpixels[i][0].getVertexL()->getEdge()->setNextwithPrev(leftColVertices[i].getEdge());

		HalfEdge* p = new HalfEdge(&leftColVertices[i], &pixelFaces[i][0]);
		VoroniVertex* nextVertex = &vertexTL;
		if (i > 0)
		{
			nextVertex = &leftColVertices[i - 1];
			nextVertex->setEdge(new HalfEdge(nextVertex, &pixelFaces[i - 1][0]));
		}
		nextVertex->getEdge()->setNextwithPrev(p);

		subpixels[i][0].getVertexL()->getEdge()->setOppPair(p);
	}

	constructSubPixelEdges();

	// After this, the bottom row and right column need to be connected to the subpixels
	for (int i = 0; i < pixelCols - 1; i++)
	{
		subpixels[pixelRows - 2][i].getVertexD()->getEdge()->getOpp()->setNextwithPrev(bottomRowVertices[i].getEdge()->getPrev());
		bottomRowVertices[i].getEdge()->getPrev()->getOpp()->setNextwithPrev(subpixels[pixelRows - 2][i].getVertexD()->getEdge());
	}
	for (int i = 0; i < pixelRows - 1; i++)
	{
		subpixels[i][pixelCols - 2].getVertexR()->getEdge()->getOpp()->setNextwithPrev(rightColVertices[i].getEdge()->getPrev());
		rightColVertices[i].getEdge()->getPrev()->getOpp()->setNextwithPrev(subpixels[i][pixelCols - 2].getVertexR()->getEdge());
	}

	// All pixel faces need the reference to an edge in their enclosing cycle
	for (int i = 0; i < pixelRows; i++)
	{
		for (int j = 0; j < pixelCols; j++)
		{
			if (j < pixelCols - 1)
			{
				if (i < pixelRows - 1)
					pixelFaces[i][j].setEdge(subpixels[i][j].getVertexU()->getEdge()->getNext());
				else
					pixelFaces[i][j].setEdge(subpixels[i - 1][j].getVertexD()->getEdge()->getNext());
			}
			else
			{
				if (i < pixelRows - 1)
					pixelFaces[i][j].setEdge(subpixels[i][j - 1].getVertexR()->getEdge()->getNext());
				else
					pixelFaces[i][j].setEdge(subpixels[i - 1][j - 1].getVertexR()->getEdge()->getOpp()->getNext());
			}
		}
	}
}

void VoroniGraph::constructSubPixelEdges()
{
	int pixelRows = sprite->getPixelRows();
	int pixelCols = sprite->getPixelCols();

	// Constructing voroni edges within each subpixel
	for (int i = 0; i < pixelRows - 1; i++)
	{
		for (int j = 0; j < pixelCols - 1; j++)
		{
			// If the subpixel has an edge from top left to bottom right
			if (graph->edgeExists(i, j, 7))
			{
				VoroniVertex* i1 = new VoroniVertex((subpixels[i][j].getVertexU()->getPosX() + subpixels[i][j].getVertexR()->getPosX()) / 2,
													(subpixels[i][j].getVertexU()->getPosY() + subpixels[i][j].getVertexR()->getPosY()) / 2);
				VoroniVertex* i2 = new VoroniVertex((subpixels[i][j].getVertexD()->getPosX() + subpixels[i][j].getVertexL()->getPosX()) / 2,
													(subpixels[i][j].getVertexD()->getPosY() + subpixels[i][j].getVertexL()->getPosY()) / 2);

				HalfEdge* e1 = new HalfEdge(subpixels[i][j].getVertexU(), &pixelFaces[i][j + 1]);
				HalfEdge* e2 = new HalfEdge(subpixels[i][j].getVertexL(), &pixelFaces[i][j]);
				HalfEdge* e3 = new HalfEdge(i1, &pixelFaces[i][j + 1]);
				HalfEdge* e4 = new HalfEdge(i2, &pixelFaces[i + 1][j + 1]);
				HalfEdge* e5 = new HalfEdge(i1, &pixelFaces[i][j]);
				HalfEdge* e6 = new HalfEdge(i2, &pixelFaces[i + 1][j]);
				
				e5->setNextwithPrev(subpixels[i][j].getVertexU()->getEdge());
				e6->setNextwithPrev(subpixels[i][j].getVertexL()->getEdge());
				i1->setEdge(new HalfEdge(i1, &pixelFaces[i + 1][j + 1]));
				i1->getEdge()->setNextwithPrev(e4);
				i2->setEdge(new HalfEdge(i2, &pixelFaces[i][j]));
				i2->getEdge()->setNextwithPrev(e5);
				e1->setNextwithPrev(e3);
				e2->setNextwithPrev(i2->getEdge());
				subpixels[i][j].getVertexU()->getEdge()->getOpp()->setNextwithPrev(e1);
				subpixels[i][j].getVertexL()->getEdge()->getOpp()->setNextwithPrev(e2);

				subpixels[i][j].getVertexR()->setEdge(new HalfEdge(subpixels[i][j].getVertexR(), &pixelFaces[i + 1][j + 1]));
				subpixels[i][j].getVertexD()->setEdge(new HalfEdge(subpixels[i][j].getVertexD(), &pixelFaces[i + 1][j]));
				subpixels[i][j].getVertexR()->getEdge()->setNextwithPrev(i1->getEdge());
				subpixels[i][j].getVertexD()->getEdge()->setNextwithPrev(e6);

				e1->setOppPair(e5);
				e2->setOppPair(e6);
				e3->setOppPair(subpixels[i][j].getVertexR()->getEdge());
				e4->setOppPair(subpixels[i][j].getVertexD()->getEdge());
				i1->getEdge()->setOppPair(i2->getEdge());
			}

			// Else if the subpixel has an edge from top right to bottom left
			else if (graph->edgeExists(i, j+1, 5))
			{
				VoroniVertex* i1 = new VoroniVertex((subpixels[i][j].getVertexU()->getPosX() + subpixels[i][j].getVertexL()->getPosX()) / 2,
								(subpixels[i][j].getVertexU()->getPosY() + subpixels[i][j].getVertexL()->getPosY()) / 2);
				VoroniVertex* i2 = new VoroniVertex((subpixels[i][j].getVertexD()->getPosX() + subpixels[i][j].getVertexR()->getPosX()) / 2,
								(subpixels[i][j].getVertexD()->getPosY() + subpixels[i][j].getVertexR()->getPosY()) / 2);

				HalfEdge* e1 = new HalfEdge(subpixels[i][j].getVertexU(), &pixelFaces[i][j + 1]);
				HalfEdge* e2 = new HalfEdge(i2, &pixelFaces[i][j + 1]);
				HalfEdge* e3 = new HalfEdge(i1, &pixelFaces[i + 1][j]);
				HalfEdge* e4 = new HalfEdge(i2, &pixelFaces[i + 1][j + 1]);
				HalfEdge* e5 = new HalfEdge(i1, &pixelFaces[i][j]);
				HalfEdge* e6 = new HalfEdge(subpixels[i][j].getVertexL(), &pixelFaces[i][j]);

				subpixels[i][j].getVertexU()->getEdge()->getOpp()->setNextwithPrev(e1);
				subpixels[i][j].getVertexL()->getEdge()->getOpp()->setNextwithPrev(e6);
				i1->setEdge(new HalfEdge(i1, &pixelFaces[i][j + 1]));
				i1->getEdge()->setNextwithPrev(e2);
				i2->setEdge(new HalfEdge(i2, &pixelFaces[i + 1][j]));
				i2->getEdge()->setNextwithPrev(e3);
				e1->setNextwithPrev(i1->getEdge());
				e3->setNextwithPrev(subpixels[i][j].getVertexL()->getEdge());
				e5->setNextwithPrev(subpixels[i][j].getVertexU()->getEdge());
				e6->setNextwithPrev(e5);

				subpixels[i][j].getVertexR()->setEdge(new HalfEdge(subpixels[i][j].getVertexR(), &pixelFaces[i + 1][j + 1]));
				subpixels[i][j].getVertexD()->setEdge(new HalfEdge(subpixels[i][j].getVertexD(), &pixelFaces[i + 1][j]));
				subpixels[i][j].getVertexR()->getEdge()->setNextwithPrev(e4);
				subpixels[i][j].getVertexD()->getEdge()->setNextwithPrev(i2->getEdge());

				e1->setOppPair(e5);
				e2->setOppPair(subpixels[i][j].getVertexR()->getEdge());
				e3->setOppPair(e6);
				e4->setOppPair(subpixels[i][j].getVertexD()->getEdge());
				i1->getEdge()->setOppPair(i2->getEdge());
			}

			// Else, there is no intersection
			else
			{
				VoroniVertex *v = new VoroniVertex(subpixels[i][j].getVertexU()->getPosX(), subpixels[i][j].getVertexL()->getPosY());
				HalfEdge* e1 = new HalfEdge(v, &pixelFaces[i][j]);
				HalfEdge* e2 = new HalfEdge(v, &pixelFaces[i][j+1]);
				HalfEdge* e3 = new HalfEdge(v, &pixelFaces[i+1][j+1]);
				HalfEdge* e4 = new HalfEdge(v, &pixelFaces[i+1][j]);
				HalfEdge* e5 = new HalfEdge(subpixels[i][j].getVertexU(), &pixelFaces[i+1][j]);
				HalfEdge* e6 = new HalfEdge(subpixels[i][j].getVertexL(), &pixelFaces[i][j]);
				v->setEdge(e1);

				subpixels[i][j].getVertexU()->getEdge()->getOpp()->setNextwithPrev(e5);
				subpixels[i][j].getVertexL()->getEdge()->getOpp()->setNextwithPrev(e6);
				e1->setNextwithPrev(subpixels[i][j].getVertexU()->getEdge());
				e4->setNextwithPrev(subpixels[i][j].getVertexL()->getEdge());
				e5->setNextwithPrev(e2);
				e6->setNextwithPrev(e1);

				subpixels[i][j].getVertexR()->setEdge(new HalfEdge(subpixels[i][j].getVertexR(), &pixelFaces[i + 1][j + 1]));
				subpixels[i][j].getVertexD()->setEdge(new HalfEdge(subpixels[i][j].getVertexD(), &pixelFaces[i + 1][j]));
				subpixels[i][j].getVertexR()->getEdge()->setNextwithPrev(e3);
				subpixels[i][j].getVertexD()->getEdge()->setNextwithPrev(e4);

				e1->setOppPair(e5);
				e2->setOppPair(subpixels[i][j].getVertexR()->getEdge());
				e3->setOppPair(subpixels[i][j].getVertexD()->getEdge());
				e4->setOppPair(e6);
			}
		}
	}
}
