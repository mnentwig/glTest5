#include <vector>
#include <math.h> // NAN
class fpsEstimator {
public:
  fpsEstimator(float averaging_s){
    this->averaging_s = averaging_s;
  }
  
  float enterFrameGetFps(float now_s){
    float tStart_s = now_s - this->averaging_s;
    unsigned int nDel = 0;
    for (unsigned int ix = 0; ix < this->frameTimes.size(); ++ix){
      if (this->frameTimes[ix] > tStart_s)
	break;
      ++nDel;
    }

    this->frameTimes.erase(this->frameTimes.begin(), this->frameTimes.begin()+nDel);
    this->frameTimes.push_back(now_s);
    if (this->frameTimes.size() > 1)
      return (float)(this->frameTimes.size()-1) / (this->frameTimes.back() - this->frameTimes.front());
    else
      return NAN;
  }
protected:
  float averaging_s;
  std::vector<float> frameTimes;
};
