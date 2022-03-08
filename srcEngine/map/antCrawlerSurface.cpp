#include "antCrawlerSurface.h"
namespace engine {
class antCrawlerSurface::neighborTable_cl {
	typedef std::tuple<triIx_t, triIx_t> edge_t;
	typedef uint64_t edgeHash_t;
public:
	void registerTri(triIx_t triIx, tri_t &tri) {
		edgeHash_t eh01 = neighborTable_cl::getEdgeHash(std::get<0>(tri), std::get<1>(tri));
		edgeHash_t eh12 = neighborTable_cl::getEdgeHash(std::get<1>(tri), std::get<2>(tri));
		edgeHash_t eh20 = neighborTable_cl::getEdgeHash(std::get<2>(tri), std::get<0>(tri));
		this->registerEdge(eh01, triIx);
		this->registerEdge(eh12, triIx);
		this->registerEdge(eh20, triIx);
	}
	bool getNeighbor(vertexIx_t va, vertexIx_t vb, triIx_t startTri, triIx_t &neighborTri) const {
		edgeHash_t eh = getEdgeHash(va, vb);
		auto it = this->table.find(eh);
		if (it == this->table.end())
			return false;
		const edge_t &e = (*it).second;
		triIx_t i1 = std::get<0>(e);
		if (i1 != startTri) {
			neighborTri = i1;
			return true;
		}
		triIx_t i2 = std::get<1>(e);
		if (i2 != startTri) {
			neighborTri = i2;
			return true;
		}
		return false;
	}
protected:
	std::map<edgeHash_t, edge_t> table;
	void registerEdge(edgeHash_t eh, triIx_t triIx) {
		auto it = this->table.find(eh);
		if (it == this->table.end()) {
			this->table[eh] = { triIx, triIx }; // one valid neighbor => set the same twice
		} else {
			edge_t &e = (*it).second;
			assert(std::get<0>(e) == std::get<1>(e)); // check for one valid neighbor
			std::get<1>(e) = triIx;
		}
	}

	static edgeHash_t getEdgeHash(vertexIx_t v1, vertexIx_t v2) {
		assert(v1 != v2);
		if (v1 < v2)
			return (edgeHash_t) v1 | (edgeHash_t) v2 << 32;
		else
			return (edgeHash_t) v2 | (edgeHash_t) v1 << 32;
	}
	static bool hasTwoNeighbors(edgeHash_t h) {
		return (h >> 32) != (h & 0xFFFFFFFF);
	}
}/*class neighborTable*/;

antCrawlerSurface::antCrawlerSurface() :
		surface() {
	this->neighborTable = new neighborTable_cl();
}

antCrawlerSurface::~antCrawlerSurface(){
	delete this->neighborTable;
}

bool antCrawlerSurface::getNeighbor(vertexIx_t va, vertexIx_t vb, triIx_t startTri, triIx_t &neighborTri) const {
	return this->neighborTable->getNeighbor(va, vb, startTri, neighborTri);
}

void antCrawlerSurface::buildNeighborTable() {
	for (unsigned int ixTri = 0; ixTri < this->tris.size(); ++ixTri) {
		this->neighborTable->registerTri(ixTri, this->tris[ixTri]);
	}
}
} // namespace
