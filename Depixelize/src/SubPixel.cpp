#include "SubPixel.h"

SubPixel::SubPixel()
{
	vertexU = NULL;
	vertexD = NULL;
	vertexL = NULL;
	vertexR = NULL;
}

SubPixel::SubPixel(VoroniVertex* U, VoroniVertex* D, VoroniVertex* L, VoroniVertex* R)
{
	vertexU = U;
	vertexD = D;
	vertexL = L;
	vertexR = R;
}

SubPixel::~SubPixel()
{
	if (vertexU != NULL)
		delete vertexU;
	if (vertexD != NULL)
		delete vertexD;
	if (vertexL != NULL)
		delete vertexL;
	if (vertexR != NULL)
		delete vertexR;
}
