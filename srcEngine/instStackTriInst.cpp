#include <string>
#include <fstream>
#include "allMyGlIncludes.h"

#include "instStackTriInst.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#define LOCATION_COORD3D 0
#define LOCATION_RGB 1
#define LOCATION_MVP_COL0 2
#define LOCATION_MVP_COL1 3
#define LOCATION_MVP_COL2 4
#define LOCATION_MVP_COL3 5

template<typename T> size_t vectorsizeof(const typename std::vector<T> &vec) {
	return sizeof(T) * vec.size();
}

static std::string get_file_string(const char *filename) {
	std::ifstream ifs(filename);
	return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
}

GLint loadShaderProgFromFile(const char *vs_filename, const char *fs_filename) {
	std::string vs = get_file_string(vs_filename);
	std::string fs = get_file_string(fs_filename);

	enum Consts {
		INFOLOG_LEN = 512
	};
	GLchar infoLog[INFOLOG_LEN];
	GLint fragment_shader;
	GLint shader_program;
	GLint success;
	GLint vertex_shader;
	const char *vshader = vs.c_str();
	const char *fshader = fs.c_str();

	/* Vertex shader */
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vshader, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, INFOLOG_LEN, NULL, infoLog);
		printf("error (%s): vertex shader compilation failed:\n%s\n", vs_filename, infoLog);
		exit(EXIT_FAILURE);
	}

	/* Fragment shader */
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fshader, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, INFOLOG_LEN, NULL, infoLog);
		printf("error (%s): fragment shader compilation failed\n%s\n", fs_filename, infoLog);
		exit(EXIT_FAILURE);
	}

	/* Link shaders */
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, INFOLOG_LEN, NULL, infoLog);
		printf("error (%s/%s): linking failed\n%s\n", vs_filename, fs_filename, infoLog);
		exit(EXIT_FAILURE);
	}

	glDeleteShader(vertex_shader); // flags for deletion on glDeleteProgram
	glDeleteShader(fragment_shader);
	return shader_program;
}

instStackTriInst::instStackTriInst() {
	this->idProgTri = loadShaderProgFromFile("shaders/instStackTri_vs.txt", "shaders/instStackTri_fs.txt");
	this->isShutdown = false;
	this->idProj = 0; // avoid warning
	this->idVertex = 0; // avoid warning
	this->idTri = 0; // avoid warning
	this->idRgb = 0; // avoid warning
}

void instStackTriInst::pushTris(std::vector<glm::vec3> vertices, std::vector<mgeng::triIx16> tris, bool rebaseZeroBasedIndices) {
	unsigned int baseVertex = this->vertexList.size();
	unsigned int firstNewTri = this->triList.size();
	this->vertexList.insert(this->vertexList.end(), vertices.begin(), vertices.end());
	this->triList.insert(this->triList.end(), tris.begin(), tris.end());

	if (rebaseZeroBasedIndices) {
		for (unsigned int ix = firstNewTri; ix < this->triList.size(); ++ix) {
			this->triList[ix].v1 += baseVertex;
			this->triList[ix].v2 += baseVertex;
			this->triList[ix].v3 += baseVertex;
		}
	}
}

unsigned int instStackTriInst::pushVertex(glm::vec3 xyz) {
	assert(!this->isFinalized);
	unsigned int retVal = this->vertexList.size();
	this->vertexList.push_back(xyz);
	return retVal;
}

void instStackTriInst::pushTri(int v1, int v2, int v3) {
	assert(!this->isFinalized);
	this->triList.push_back( { (GLushort) v1, (GLushort) v2, (GLushort) v3 });
}

unsigned int instStackTriInst::getTriCount() const {
	assert(!this->isFinalized);
	return this->triList.size();
}

void instStackTriInst::pushTwoTri(int v1, int v2, int v3, int v4) {
	assert(!this->isFinalized);
	this->triList.push_back( { (GLushort) v1, (GLushort) v2, (GLushort) v3 });
	this->triList.push_back( { (GLushort) v1, (GLushort) v3, (GLushort) v4 });
}

void instStackTriInst::finalize() {
	assert(!this->isFinalized);
	this->isFinalized = true;

	GL_CHK(glGenBuffers(1, &this->idRgb));
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idRgb));

	GL_CHK(glGenBuffers(1, &this->idProj));
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idProj));

	GL_CHK(glGenBuffers(1, &this->idVertex));
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
	GL_CHK(glBufferData(GL_ARRAY_BUFFER, vectorsizeof(this->vertexList), this->vertexList.data(), GL_STATIC_DRAW));

	GL_CHK(glGenBuffers(1, &this->idTri));
	GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idTri));
	GL_CHK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, vectorsizeof(this->triList), this->triList.data(), GL_STATIC_DRAW));

	// === cleanup ===
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
void instStackTriInst::runOverlay(const glm::mat4 *const proj, const glm::vec3 *const rgb, int nInst) const {
	GL_CHK(glDisable (GL_DEPTH_TEST));
	this->run(proj, rgb, nInst);
	GL_CHK(glEnable (GL_DEPTH_TEST));
}

void instStackTriInst::run(const glm::mat4 *const proj, const glm::vec3 *const rgb, int nInst) const {
	assert(this->isFinalized);
	if (this->triList.size() < 1)
		return;
	if (nInst < 1)
		return;

	// === COORD3D: per triangle ===
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
	GL_CHK(glEnableVertexAttribArray(LOCATION_COORD3D));
	GL_CHK(glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0));
	GL_CHK(glVertexAttribDivisor(LOCATION_COORD3D, 0));

	// === RGB: per instance ===
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idRgb));
	GL_CHK(glBufferData(GL_ARRAY_BUFFER, nInst * sizeof(glm::vec3), (void*)rgb, GL_STATIC_DRAW));
	GL_CHK(glEnableVertexAttribArray(LOCATION_RGB));
	GL_CHK(glVertexAttribPointer(LOCATION_RGB, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0));
	GL_CHK(glVertexAttribDivisor(LOCATION_RGB, 1));

	// === proj: per instance (4 rows) ===
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idProj));
	GL_CHK(glBufferData(GL_ARRAY_BUFFER, nInst * sizeof(glm::mat4), (void*)proj, GL_STATIC_DRAW));

	GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL0));
	GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0*sizeof(glm::vec4))));
	GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL0, 1));

	GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL1));
	GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1*sizeof(glm::vec4))));
	GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL1, 1));

	GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL2));
	GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4))));
	GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL2, 1));

	GL_CHK(glEnableVertexAttribArray(LOCATION_MVP_COL3));
	GL_CHK(glVertexAttribPointer(LOCATION_MVP_COL3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3*sizeof(glm::vec4))));
	GL_CHK(glVertexAttribDivisor(LOCATION_MVP_COL3, 1));

	// === program ===
	GL_CHK(glUseProgram(this->idProgTri));

	// === vertex indices ===
	GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idTri));

	// === draw ===
	GL_CHK(glDrawElementsInstanced(GL_TRIANGLES, this->triList.size()*3, GL_UNSIGNED_SHORT, 0, nInst));

	// === cleanup ===
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GL_CHK(glDisableVertexAttribArray(LOCATION_COORD3D));
	GL_CHK(glDisableVertexAttribArray(LOCATION_RGB));
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL0));
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL1));
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL2));
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL3));
	GL_CHK(glUseProgram(0));
}

void instStackTriInst::run1(unsigned int ixTri1, unsigned int ixTri2, const glm::mat4 &proj, const glm::vec3 &rgb) const {
	assert(this->isFinalized);

	// === COORD3D: per triangle ===
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, this->idVertex));
	GL_CHK(glEnableVertexAttribArray(LOCATION_COORD3D));
	GL_CHK(glVertexAttribPointer(LOCATION_COORD3D, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0));
	GL_CHK(glVertexAttribDivisor(LOCATION_COORD3D, 0));

	// === RGB ===
	GL_CHK(glDisableVertexAttribArray(LOCATION_RGB));
	GL_CHK(glVertexAttrib3fv(LOCATION_RGB, (float*)&rgb));

	// === proj ===
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL0));
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL1));
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL2));
	GL_CHK(glDisableVertexAttribArray(LOCATION_MVP_COL3));
	GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL0, &proj[0][0]));
	GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL1, &proj[1][0]));
	GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL2, &proj[2][0]));
	GL_CHK(glVertexAttrib4fv(LOCATION_MVP_COL3, &proj[3][0]));

	// === program ===
	GL_CHK(glUseProgram(this->idProgTri));

	// === vertex indices ===
	GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->idTri));

	// === draw ===
	// note: https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glDrawElements.xhtml "indices specifies a byte offset..."
	unsigned long long byteOffset = ixTri1 * 3 * sizeof(GLshort);
	GL_CHK(glDrawElements(GL_TRIANGLES, (ixTri2-ixTri1)*3, GL_UNSIGNED_SHORT, (void*)byteOffset));

	// === cleanup ===
	GL_CHK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GL_CHK(glDisableVertexAttribArray(LOCATION_COORD3D));
	GL_CHK(glUseProgram(0));
}

void instStackTriInst::reset() {
	if (this->isFinalized) {
		GL_CHK(glDeleteBuffers(1, &this->idTri));
		GL_CHK(glDeleteBuffers(1, &this->idVertex));
		this->vertexList.clear();
		this->triList.clear();
		this->isFinalized = false;
	}
}

void instStackTriInst::shutdown() {
	assert(!this->isShutdown);
	this->isShutdown = true;
	this->reset();
	GL_CHK(glDeleteProgram(this->idProgTri));
}

instStackTriInst::~instStackTriInst() {
	assert(this->isShutdown);
}
