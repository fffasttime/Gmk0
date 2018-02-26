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

#ifndef GTP_H_INCLUDED
#define GTP_H_INCLUDED

#include "config.h"

#include <cstdio>
#include <string>
#include <vector>

//#include "GameState.h"

extern int cfg_num_threads;
extern int cfg_max_playouts;
extern std::uint64_t cfg_rng_seed;
#ifdef USE_OPENCL
extern std::vector<int> cfg_gpus;
extern bool cfg_sgemm_exhaustive;
extern bool cfg_tune_only;
#endif
extern float cfg_softmax_temp;
extern std::string cfg_weightsfile;
extern std::string cfg_options_str;
extern std::string cfg_curr_dir;
extern bool cfg_quiet;

class GTP {
public:
    static void setup_default_parameters();
private:
    static constexpr int GTP_VERSION = 2;
};


#endif
