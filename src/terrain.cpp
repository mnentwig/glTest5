#include "terrain.h"
#include <sys/stat.h>
#include <stdexcept>
#include <cmath> // sqrt
#include <fstream>
#include "myGl.h"
#include <vector>
#include <iostream>
#include "utils.h" // getTime

#define LOCATION_COORD3D 0
#define LOCATION_RGB 1
#define LOCATION_MVP_COL0 2
#define LOCATION_MVP_COL1 3
#define LOCATION_MVP_COL2 4
#define LOCATION_MVP_COL3 5
#define LOCATION_CAMERA 6
#define LOCATION_DISTFACTOR 7

terrain::terrain(const char* filename, terrTriDomain& d){

  printf ("%1.3f\t%s\n", getTime (), "loading Terrain");
  struct stat results;
// === get nElem/nBytes ===
  if (stat (filename, &results) != 0) throw std::runtime_error ("failed to open heightfield");
  unsigned int nBytes = results.st_size;
  unsigned int nElem = nBytes / sizeof(unsigned short);
  if (nElem * sizeof(unsigned short) != nBytes) throw std::runtime_error ("file length not multiple of number size");

// === determine map size ===
  this->nX = sqrt (nElem);
  this->nY = sqrt (nElem);
  if (this->nX * this->nY != nElem) throw std::runtime_error ("heightmap size not square");

// === load data ===
  unsigned short *data = new unsigned short[nElem];
  std::ifstream f (filename, std::ios::in | std::ios::binary);
  if (f.rdstate () & std::ifstream::failbit) std::runtime_error ("heightmap open failed");

  if (!f.read ((char*) data, nBytes)) std::runtime_error ("heightmap read failed");
  if (f.gcount () != nBytes) throw std::runtime_error ("heightmap read failed (nbytes)");
  f.close ();

  printf ("%1.3f\t%s\n", getTime (), "registering tris");
  std::vector<glm::vec3> trilistOdd;
  std::vector<glm::vec3> trilistEven;
  std::vector<glm::vec3> *l;

//  this->nX = 30; this->nY = 30; // !!!

// === create vertices ===
  d.reserveVertexSpace(this->nX * this->nY);
  for (unsigned int ixY = 0; ixY < this->nY; ++ixY) {
    for (unsigned int ixX = 0; ixX < this->nX; ++ixX) {
      glm::vec3 vPos = this->getVertex (data, ixX, ixY);
      terrTriVertIx vIx = this->getVertexIx (ixX, ixY);
      d.setVertex (vIx, vPos);
    }
  }

  // === create tris ===
  for (unsigned int ixY = 0; ixY < this->nY - 1; ++ixY) {
    for (unsigned int ixX = 0; ixX < this->nX - 1; ++ixX) {
      terrTriVertIx ne = this->getVertexIx (ixX, ixY);
      terrTriVertIx se = this->getVertexIx (ixX, ixY + 1);
      terrTriVertIx nw = this->getVertexIx (ixX + 1, ixY);
      terrTriVertIx sw = this->getVertexIx (ixX + 1, ixY + 1);
      l = (ixX + ixY) % 2 ? &trilistOdd : &trilistEven;
      l->push_back (d.getVertex (ne));
      l->push_back (d.getVertex (se));
      l->push_back (d.getVertex (nw));
      d.registerTri (ne, se, nw);

      l->push_back (d.getVertex (nw));
      l->push_back (d.getVertex (se));
      l->push_back (d.getVertex (sw));
     // d.registerTri (nw, se, sw);
      d.registerTri (sw, nw, se);
    }
  }
  printf ("%1.3f\t%s\n", getTime (), "openGl buffers");
  this->idProg = loadShaderProgFromFile ("shaders/terrain_vs.txt", "shaders/instStackTri_fs.txt");

  this->nTriOdd = trilistOdd.size () / 3;
  GL_CHK(glGenBuffers (1, &this->idTriOdd));
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idTriOdd));
  GL_CHK(glBufferData(GL_ARRAY_BUFFER, this->nTriOdd*sizeof(glm::vec3)*3, trilistOdd.data(), GL_STATIC_DRAW));

  this->nTriEven = trilistEven.size () / 3;
  GL_CHK(glGenBuffers (1, &this->idTriEven));
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idTriEven));
  GL_CHK(glBufferData(GL_ARRAY_BUFFER, this->nTriEven*sizeof(glm::vec3)*3, trilistEven.data(), GL_STATIC_DRAW));

  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  printf ("%1.3f\t%s\n", getTime (), "terrain done");

  delete data;
}

void terrain::render(glm::mat4& proj, glm::vec3 camera, float maxRenderDistance, glm::vec3& colOdd, glm::vec3& colEven){
  GL_CHK(glEnable(GL_CULL_FACE));
  GL_CHK(glCullFace(GL_BACK));
  GL_CHK(glFrontFace(GL_CCW));

// === COORD3D: per triangle ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idTriOdd));
  GL_CHK(glEnableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0));

// === RGB: per instance ===
  GL_CHK(glVertexAttrib3f(LOCATION_RGB, colOdd[0], colOdd[1], colOdd[2]));
  GL_CHK(glDisableVertexAttribArray(LOCATION_RGB));

// === proj: per instance (4 rows) ===
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL0, proj[0][0], proj[0][1], proj[0][2], proj[0][3]));
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL1, proj[1][0], proj[1][1], proj[1][2], proj[1][3]));
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL2, proj[2][0], proj[2][1], proj[2][2], proj[2][3]));
  GL_CHK(glVertexAttrib4f(LOCATION_MVP_COL3, proj[3][0], proj[3][1], proj[3][2], proj[3][3]));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL1));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL2));
  GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL3));

// === camera position ===
  GL_CHK(glVertexAttrib3f(LOCATION_CAMERA, camera[0], camera[1], camera[2]));
  GL_CHK(glDisableVertexAttribArray(LOCATION_CAMERA));

// === max. render distance ===
  float distFactor = 1.0f / maxRenderDistance;
  distFactor *= distFactor;
  GL_CHK(glVertexAttrib1f(LOCATION_DISTFACTOR, distFactor));
  GL_CHK(glDisableVertexAttribArray(LOCATION_DISTFACTOR));

// === program ===
  GL_CHK(glUseProgram (this->idProg));

// === draw odd ===
  GL_CHK(glDrawArrays(GL_TRIANGLES, 0, this->nTriOdd*3));

// === draw even ===
// re-uses most settings above
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idTriEven));
  GL_CHK(glEnableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0));
  GL_CHK(glVertexAttrib3f(LOCATION_RGB, colEven[0], colEven[1], colEven[2]));
  GL_CHK(glDrawArrays(GL_TRIANGLES, 0, this->nTriEven*3));

// === cleanup ===
  GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GL_CHK(glDisableVertexAttribArray(LOCATION_COORD3D));
  GL_CHK(glUseProgram (0));
  GL_CHK(glDisable(GL_CULL_FACE));
}

terrTriVertIx terrain::getVertexIx(unsigned int ixX, unsigned int ixY) const{
  return ixY * this->nX + ixX;
}

glm::vec3 terrain::getVertex(unsigned short* data, unsigned int ixX, unsigned int ixY) const{
  assert(ixX < this->nX);
  assert(ixY < this->nY);
  unsigned short zi = *(data + ixY * this->nX + ixX);
  float x = ((float) ixX) - this->nX / 2;
  float z = ((float) ixY) - this->nY / 2;
  float y = (zi - 65535.0f) / 300.0f;
  const float scale = 5.0f;
  x *= scale;
  y *= scale;
  z *= scale;

  return glm::vec3 (x, y, z);
}

