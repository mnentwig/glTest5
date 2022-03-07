#pragma once
#include "surface.h"
#include <map>
namespace engine {
class antCrawlerSurface: public surface {
	class neighborTable_cl;
public:
	antCrawlerSurface();
	bool getNeighbor(vertexIx_t va, vertexIx_t vb, triIx_t startTri, triIx_t &neighborTri) const;
	~antCrawlerSurface();

protected:
	neighborTable_cl* neighborTable;
	void buildNeighborTable();
};
}
