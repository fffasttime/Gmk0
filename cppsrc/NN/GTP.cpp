/*
    Extended code from:
    This file is part of Leela Zero.
    Copyright (C) 2017 Gian-Carlo Pascutto

    Leela Zero is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leela Zero is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Leela Zero.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "GTP.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "Network.h"
#include "SMP.h"
#include "Utils.h"

using namespace Utils;

// Configuration flags
int cfg_num_threads;
int cfg_max_playouts;
std::uint64_t cfg_rng_seed;
#ifdef USE_OPENCL
std::vector<int> cfg_gpus;
bool cfg_sgemm_exhaustive;
bool cfg_tune_only;
#endif
float cfg_softmax_temp;
std::string cfg_weightsfile;
std::string cfg_options_str;
std::string cfg_curr_dir;
bool cfg_quiet;

void GTP::setup_default_parameters() {
    cfg_num_threads = std::max(1, std::min(SMP::get_num_cpus(), MAX_CPUS));
    cfg_max_playouts = std::numeric_limits<decltype(cfg_max_playouts)>::max();
#ifdef USE_OPENCL
    cfg_gpus = { };
    cfg_sgemm_exhaustive = false;
    cfg_tune_only = false;
#endif
    cfg_softmax_temp = 1.0f;
    // C++11 doesn't guarantee *anything* about how random this is,
    // and in MinGW it isn't random at all. But we can mix it in, which
    // helps when it *is* high quality (Linux, MSVC).
    std::random_device rd;
    std::ranlux48 gen(rd());
    std::uint64_t seed1 = (gen() << 16) ^ gen();
    // If the above fails, this is one of our best, portable, bets.
    std::uint64_t seed2 = std::chrono::high_resolution_clock::
        now().time_since_epoch().count();
    cfg_rng_seed = seed1 ^ seed2;
}