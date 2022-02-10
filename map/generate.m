rand('state', 1);
nPts = 10;
x = rand (1, nPts);
y = rand (1, nPts);
z = rand (1, nPts);
tri = delaunay (x, y);
triplot (tri, x, y);
hold on;
plot (x, y, "r*");
axis ([0,1,0,1]);

h = fopen('generated.h', 'w'); assert(h > 0);
fprintf(h, '//std::vector<glm::vec3> vertices;\n', nPts);
for ix = 1 : nPts
  fprintf(h, 'vertices.push_back(glm::vec3(%f, %f, %f));\n', x(ix), z(ix), y(ix));
end

fprintf(h, '//std::vector<std::tuple<unsigned int, unsigned int, unsigned int>> tris;\n');
for ix = 1 : size(tri, 1)
  fprintf(h, 'tris.push_back(std::tuple<unsigned int, unsigned int, unsigned int>({%i, %i, %i}));\n', tri(ix, 1)-1, tri(ix, 2)-1, tri(ix, 3)-1);
end

fclose(h);
