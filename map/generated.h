//std::vector<glm::vec3> vertices;
vertices.push_back(glm::vec3(0.134364, 0.025446, 0.835765));
vertices.push_back(glm::vec3(0.847434, 0.541412, 0.432767));
vertices.push_back(glm::vec3(0.763775, 0.939149, 0.762280));
vertices.push_back(glm::vec3(0.255069, 0.381204, 0.002106));
vertices.push_back(glm::vec3(0.495435, 0.216599, 0.445387));
vertices.push_back(glm::vec3(0.449491, 0.422117, 0.721540));
vertices.push_back(glm::vec3(0.651593, 0.029041, 0.228762));
vertices.push_back(glm::vec3(0.788723, 0.221692, 0.945271));
vertices.push_back(glm::vec3(0.093860, 0.437888, 0.901427));
vertices.push_back(glm::vec3(0.028347, 0.495812, 0.030590));
//std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> tris;
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({2, 7, 1}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({2, 5, 7}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({2, 4, 1}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({2, 4, 5}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({0, 4, 9}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({0, 4, 5}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({6, 4, 1}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({8, 5, 7}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({8, 0, 5}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({8, 0, 9}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({3, 4, 9}));
tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({3, 6, 4}));
