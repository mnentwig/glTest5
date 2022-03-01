#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
class plotly {
public:
	plotly(std::string filename) {
		this->filename = filename;
	}

	void close() {
		std::ofstream f;
		f.open(this->filename, std::ios::out | std::ios::binary);

		std::stringstream s;
		for (auto iter = this->vectors.begin(); iter != this->vectors.end(); ++iter) {
			s << "var " << iter->first << " = [" << iter->second.str() << "];\n";
		}

		f << "<!DOCTYPE html>\n";
		f << "<html>\n";
		f << "<script src=\"plotly-2.9.0.min.js\"></script>\n";
		f << "<body>\n";
		f << "<div id=\"myPlot\"></div>\n";
		f << "<script>\n";
		f << s.str();
		f << "var data = [{type:\"mesh3d\", x:triVertexX, y: triVertexY, z:triVertexZ, i: triVertexA, j:triVertexB, k:triVertexC, facecolor: triColor, lighting: {ambient:1, diffuse:0}}];\n";
		f << "var layout = {autosize:false, width: 800, height:800};\n";
		f << "var config = {displaylogo: false};\n";
		f << "Plotly.newPlot(\"myPlot\", data, layout, config);\n";
		f << "</script>\n";
		f << "</body>\n";
		f << "</html>\n";
		f.close();
	}

	template<typename T> void appendVec(std::string vecName, T value) {
		auto iter = this->vectors.emplace(vecName, "");
		bool entryIsNew = iter.second;
		std::stringstream &ss = iter.first->second;
		if (!entryIsNew)
			ss << ",";
		ss << value;
	}
protected:
	std::string filename;
	std::map<std::string, std::stringstream> vectors;
};
