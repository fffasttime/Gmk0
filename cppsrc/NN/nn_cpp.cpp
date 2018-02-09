#include "nn_cpp.h"

NN::NN(std::string weight_file)
{
	myprintf("finding weight file %s \n", weight_file.c_str());
	cfg_weightsfile = weight_file;
	GTP::setup_default_parameters();
	init_global_objects();
}

void NN::set_weights(std::string weight_file)
{
	cfg_weightsfile = weight_file;
}

void NN::set_threads(int thread_num)
{
	myprintf("Using %d thread(s).\n", thread_num);
	cfg_num_threads = thread_num;
}

void NN::set_gpu(int gpu_num)
{
	cfg_gpus = std::vector<int>(gpu_num);
}

void NN::set_tune_opt(bool tune_only)
{
	cfg_tune_only = tune_only;
	cfg_sgemm_exhaustive = !tune_only;
}

void NN::init_global_objects()
{
	thread_pool.initialize(cfg_num_threads);

	// Use deterministic random numbers for hashing
	auto rng = std::make_unique<Random>(5489);

	// Initialize the main thread RNG.
	// Doing this here avoids mixing in the thread_id, which
	// improves reproducibility across platforms.
	Random::get_Rng().seedrandom(cfg_rng_seed);

	NNCache::get_NNCache().set_size_from_playouts(cfg_max_playouts);

	// Initialize network
	Network::initialize();
}

void NN::set_maxplayout(int maxplayout)
{
	cfg_max_playouts = maxplayout;
}

Network::NN_Ouputs NN::forward(Network::NNPlanes input_planes)
{
	return Network::nn_forward(input_planes);
}


