#include "antCrawler.h"
#include "srcEngine/geomUtils2d.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/intersect.hpp>
namespace engine {
antCrawler::antCrawler(antCrawlerSurface *surface, float xInit, float zInit) {
	this->surface = surface;

	unsigned int droppedTriIx = this->locateTriByVerticalDrop(xInit, zInit);
	this->setTri(droppedTriIx);
	this->pos_2d = this->getPosByVerticalDrop(xInit, zInit);
}

const glm::vec3& antCrawler::getPos() {
	this->posCache_3d = this->m2dTo3d * glm::vec3(this->pos_2d, this->commonZ);
	return this->posCache_3d;
}

enum moveResult_e {
	/// movement ended in same tri as it started
	SAME_TRI,
	/// movement ended in a different tri than where it started
	OTHER_TRI,
	/// movement over the edge (clipped)
	BORDER
};

antCrawler::moveResult_e antCrawler::move(const glm::vec3 delta) { // return quat?
	glm::vec2 dir_2d = this->m3dTo2d * delta;
	moveResult_e res = moveResult_e::SAME_TRI;
	// all three edges are checked for intersection
	edgeHash_t edgeUnderPos2d = 0;
	while (true) {
		glm::vec2 prelimNewPos_2d = this->pos_2d + dir_2d;

		// === check whether the movement crosses an edge (leaves the triangle) ===
		/// if crossed, how much movement is left unspent along the edge?
		float remLengthParallelToEdge;
		/// if crossed, how much movement is left unspent perpendicular to the edge (towards the neighbor's 3rd point)
		float remLengthOrthogonalToEdge;
		/// first edge vertex number (valid regardless of current tri)
		surface::vertexIx_t commonEdgeVertexNumA;
		/// second edge vertex number (valid regardless of current tri)
		surface::vertexIx_t commonEdgeVertexNumB;
		/// intersection location between common edge VA/VB
		float isBaryAB;
		// identified neighbor
		surface::triIx_t neighborTri;
		{
			/// internal index (0..2) of the first point of the crossed edge (valid only for the original tri)
			unsigned int origIxVA_0to2;
			/// internal index (0..2) of the first point of the crossed edge (valid only for the original tri)
			unsigned int origIxVB_0to2;
			for (origIxVA_0to2 = 0; origIxVA_0to2 < 3; ++origIxVA_0to2) {
				origIxVB_0to2 = origIxVA_0to2 < 2 ? origIxVA_0to2 + 1 : 0;
				// === skip the edge we're on (if any) ===
				// note: This happens only when it is known that movement entered through said edge, therefore it cannot leave through the same edge.
				edgeHash_t eh = getEdgeHash(this->v_ix[origIxVA_0to2], this->v_ix[origIxVB_0to2]);
				if (eh == edgeUnderPos2d)
					continue;

				if (testCrossingAndProjectOnEdge(this->v_2d[origIxVA_0to2], this->v_2d[origIxVB_0to2], this->pos_2d, prelimNewPos_2d, /*out*/isBaryAB, remLengthParallelToEdge, remLengthOrthogonalToEdge)) {
					// pos_2d will move onto this edge, therefore exclude it from the next intersection check
					edgeUnderPos2d = eh;
					goto edgeCrossed;
				}
			}

			// === movement ends in current tri => we're done ===
			this->pos_2d = prelimNewPos_2d;
			return res; // ----------------------------------------------------------------------------------------

			edgeCrossed: commonEdgeVertexNumA = this->v_ix[origIxVA_0to2];
			commonEdgeVertexNumB = this->v_ix[origIxVB_0to2];

			// === Identify neighbor on the crossed edge ===
			if (!this->surface->getNeighbor(commonEdgeVertexNumA, commonEdgeVertexNumB, this->currentTriIx, /*out*/neighborTri)) {
				// === no neighbor exists. Set new position on border edge ===
				this->pos_2d = (1.0f - isBaryAB) * this->v_2d[origIxVA_0to2] + isBaryAB * this->v_2d[origIxVB_0to2];
				// leave
				return moveResult_e::BORDER;
			}
		}

		// === Load the neighbor tri ===
		// Note: As the 3d-to-2d mapping changes with the inclination of the new tri, 2d coordinates become invalid
		res = moveResult_e::OTHER_TRI;
		this->setTri(neighborTri);

		// === identify the vertices of the common edge on the neighbor = new "current" tri ===
		int internalIxA_0to2 = this->identifyVertex(commonEdgeVertexNumA); // first point on neighbor edge
		int internalIxB_0to2 = this->identifyVertex(commonEdgeVertexNumB); // second point on neighbor edge
		int internalIxC_0to2 = this->thirdInternalIx(internalIxA_0to2, internalIxB_0to2); // non-adjacent point

		// === look up 2d vertices ===
		const glm::vec2 &commonEdgeVertexPosA = this->v_2d[internalIxA_0to2];
		const glm::vec2 &commonEdgeVertexPosB = this->v_2d[internalIxB_0to2];
		const glm::vec2 &vertexPosC = this->v_2d[internalIxC_0to2];

		// === use the barycentric coefficient for ptA-ptB to determine the new 2d position ===
		this->pos_2d = (1.0f - isBaryAB) * commonEdgeVertexPosA + isBaryAB * commonEdgeVertexPosB;

		// === calculate unit vectors of the new tri corresponding to remLengthParallelToEdge / remLengthOrthogonalToEdge
		/// unit vector in AB direction
		glm::vec2 unitDirParallelToEdge;
		/// unit vector orthogonal to AB, pointing towards C
		glm::vec2 unitDirOrthogonalToEdge;
		geomUtils2d::orthogonalize(commonEdgeVertexPosA, commonEdgeVertexPosB, vertexPosC, /*out*/unitDirParallelToEdge, unitDirOrthogonalToEdge);

		// === update direction ===
		dir_2d = remLengthParallelToEdge * unitDirParallelToEdge + remLengthOrthogonalToEdge * unitDirOrthogonalToEdge;
	}
}

int antCrawler::identifyVertex(surface::vertexIx_t targetIx) {
	for (int internalIx = 0; internalIx < 3; ++internalIx)
		if (this->v_ix[internalIx] == targetIx)
			return internalIx;
	throw std::runtime_error("identify vertex: not found");
}

int antCrawler::thirdInternalIx(int internalIxA, int internalIxB) {
	if ((internalIxA == 0) && (internalIxB == 1))
		return 2;
	if ((internalIxA == 0) && (internalIxB == 2))
		return 1;
	if ((internalIxA == 1) && (internalIxB == 0))
		return 2;
	if ((internalIxA == 1) && (internalIxB == 2))
		return 0;
	if ((internalIxA == 2) && (internalIxB == 0))
		return 1;
	if ((internalIxA == 2) && (internalIxB == 1))
		return 0;
	throw std::runtime_error("identify vertex: invalid args");
}

unsigned int antCrawler::locateTriByVerticalDrop(float xInit, float zInit) const {
	const glm::vec3 *v0;
	const glm::vec3 *v1;
	const glm::vec3 *v2;
	for (unsigned int triIx = 0; triIx < this->surface->tris.size(); ++triIx) {
		this->surface->getTri(triIx, &v0, &v1, &v2);
		if (geomUtils2d::pointInTriangleNoY(glm::vec3(xInit, 0, zInit), *v0, *v1, *v2)) {
			return triIx;
		}
	}
	throw std::runtime_error("failed to locate tri by vertical drop");
}

glm::vec2 antCrawler::getPosByVerticalDrop(float xInit, float zInit) const {
	const glm::vec3 *v0;
	const glm::vec3 *v1;
	const glm::vec3 *v2;
	this->surface->getTri(this->currentTriIx, &v0, &v1, &v2);

	// Need all three coordinates of the initial point to transform to 2d
	// Note: The "ray" in intersectRayTriangle is bidirectional.
	// With the startpoint at y=0, "dist" gives  the y-coordinate.
	glm::vec3 rayOrig = glm::vec3(xInit, 0, zInit);
	glm::vec3 rayDir = glm::vec3(0, 1, 0);
	glm::vec2 baryPosition;
	float dist=-1;
	glm::intersectRayTriangle(rayOrig, rayDir, *v0, *v1, *v2, baryPosition, dist);
	glm::vec3 pos_3d(xInit, dist, zInit);
	return this->m3dTo2d * pos_3d;
}

void antCrawler::setTri(surface::triIx_t triIx) {
	this->currentTriIx = triIx;

	const glm::vec3 *v0;
	const glm::vec3 *v1;
	const glm::vec3 *v2;
	this->surface->getTri(triIx, &v0, &v1, &v2);

	this->surface->getVertexIx(triIx, /*out*/this->v_ix[0], this->v_ix[1], this->v_ix[2]);
	// === normal vector in 3d ===
	const glm::vec3 normalTri = glm::normalize(glm::cross(*v1 - *v0, *v2 - *v0));
	// === targeted normal vector in 2d when z is eliminated ===
	const glm::vec3 normalNoZ = glm::vec3(0, 0, 1);
	// === rotation axis to rotate normalTri into normalNoZ ===
	const glm::vec3 axis = glm::normalize(glm::cross(normalTri, normalNoZ));
	// === rotation angle to rotate normalTri into normalNoZ ===
	const float phi_rad = std::acos(glm::dot(normalTri, normalNoZ));
	// === rotation matrix to rotate normalTri into normalNoZ ===
	this->m3dTo2d = glm::rotate(glm::mat4(1.0f), phi_rad, axis);
	// === rotation matrix to rotate normalNoZ into normalTri ===
	this->m2dTo3d = glm::inverse(this->m3dTo2d);

	// === transform 3d triangle to 2d with constant z ===
	glm::vec3 v0_2dPlusZ = this->m3dTo2d * (*v0);
	this->commonZ = v0_2dPlusZ.z; // keep z from one arbitrary vertex
	this->v_2d[0] = v0_2dPlusZ;
	this->v_2d[1] = this->m3dTo2d * (*v1); // other vertices z will be identical, no need to calculate
	this->v_2d[2] = this->m3dTo2d * (*v2);
}

/*static*/ bool antCrawler::lineLineIntersection(const glm::vec2 &p1, const glm::vec2 &p2, const glm::vec2 &p3, const glm::vec2 &p4, glm::vec2 &out_tu) {
	const float dx13 = (p1.x - p3.x);
	const float dy13 = (p1.y - p3.y);
	const float dy34 = (p3.y - p4.y);
	const float dx34 = (p3.x - p4.x);
	const float dx12 = (p1.x - p2.x);
	const float dy12 = (p1.y - p2.y);
	float denom = dx12 * dy34 - dy12 * dx34;
	float num_t = dx13 * dy34 - dy13 * dx34;
	float num_u = dx13 * dy12 - dy13 * dx12;
	if (fabs(denom) <= std::numeric_limits<float>::epsilon())
		return false;
	float t = num_t / denom;
	if ((t < 0) || (t > 1))
		return false;
	float u = num_u / denom;
	if ((u < 0) || (u > 1))
		return false;
	out_tu = glm::vec2(t, u);
	return true;
}

/// checks whether the line B=[startpt, endpt] crosses the line A=[v0, v1],
/// Returns true, if intersection. In this case, the remaining length of B is returned projected on A and its normal.
bool antCrawler::testCrossingAndProjectOnEdge(const glm::vec2 &v0, const glm::vec2 &v1, const glm::vec2 &startpt, const glm::vec2 &endpt, float &isBary01, float &remLengthOnv0v1, float &remLengthOnv0v1normal) {
	glm::vec2 out_tu;
	if (!lineLineIntersection(v0, v1, startpt, endpt, out_tu))
		return false;

	// === calculate intersection point from returned barycentric coordinates ===
	isBary01 = out_tu[0];
	glm::vec2 isPt = (1.0f - isBary01) * v0 + isBary01 * v1;
	glm::vec2 rem = endpt - isPt;

	// === project rem on [v0, v1] ===
	glm::vec2 v0v1norm = glm::normalize(v1 - v0);
	float f = glm::dot(v0v1norm, rem);
	glm::vec2 remParallel = v0v1norm * f;
	glm::vec2 remOrthogonal = rem - remParallel;
	remLengthOnv0v1 = f; // note, negative values are possible as the vector direction must be from A to B
	remLengthOnv0v1normal = glm::length(remOrthogonal);

	return true;
}
/*static*/ antCrawler::edgeHash_t antCrawler::getEdgeHash(surface::vertexIx_t v1, surface::vertexIx_t v2) {
	assert(v1 != v2);
	if (v1 < v2)
		return (edgeHash_t) v1 | (edgeHash_t) v2 << 32;
	else
		return (edgeHash_t) v2 | (edgeHash_t) v1 << 32;
}
}
