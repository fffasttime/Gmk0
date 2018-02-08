
#ifndef NN_CPP_H_INCLUDED
#define NN_CPP_H_INCLUDED

#include "config.h"

#include <cstdint>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "GTP.h"
#include "Network.h"
#include "NNCache.h"
#include "Random.h"
#include "ThreadPool.h"
#include "Utils.h"

using namespace Utils;

class NN
{
public:
	NN(std::string weight_file) ;
	void set_weights(std::string weight_file);
	void set_threads(int thread_num);
	void set_gpu(int gpu_num);
	void set_tune_opt(bool tune_only);
	void init_global_objects();
	void set_maxplayout(int maxplayout);
	Network::NN_Ouputs forward(Network::NNPlanes input_planes);
};


#endif
