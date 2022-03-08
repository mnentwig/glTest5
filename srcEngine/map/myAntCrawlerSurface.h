#include "antCrawlerSurface.h"
#include "antCrawler.h"
#include "srcEngine/util/plotly.hpp"
namespace engine {
class myAntCrawlerSurface: public antCrawlerSurface {
public:
	myAntCrawlerSurface();
	void plot(plotly &p) const;
};
}
