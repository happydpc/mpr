/*
Reference implementation for
"Massively Parallel Rendering of Complex Closed-Form Implicit Surfaces"
(SIGGRAPH 2020)

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (C) 2019-2020  Matt Keeter
*/
#pragma once
#include <cstdint>
#include <Eigen/Eigen>

#include "util.hpp"

namespace mpr {

// Forward declaration
struct Tape;

struct TileNode {
    int32_t position;
    int32_t tape;
    int32_t next;
};

struct Tiles {
    /* 2D array of filled Z values (or 0) */
    Ptr<int32_t[]> filled;

    /*  1D list of active tiles */
    Ptr<TileNode[]> tiles;
    size_t tile_array_size=0;
};

struct Context {
    Context(int32_t image_size_px);
    void render3D(const Tape& tape, const Eigen::Matrix4f& mat);
    void render2D(const Tape& tape, const Eigen::Matrix3f& mat,
                  const float z=0.0f);

    /*  Renders a 2D image using a brute-force approach, without subdivision
     *  or tape pruning.  This is only useful for benchmarking, and is not
     *  recommended for regular use. */
    void render2D_brute(const Tape& tape,
                        const Eigen::Matrix3f& mat,
                        const float z=0.0f);

    /*  Renders a 2D image, accumulating amortized work per pixel in a heatmap.
     *  This is used to generate a figure in the research paper, and is not
     *  recommended for regular use. */
    Ptr<float[]> render2D_heatmap(const Tape& tape,
                                  const Eigen::Matrix3f& mat,
                                  const float z=0.0f);
    Ptr<float[]> render3D_heatmap(const Tape& tape,
                                  const Eigen::Matrix4f& mat);

    int32_t image_size_px;

    Ptr<uint64_t[]> tape_data;    // original tape is copied to index 0
    Ptr<int32_t> tape_index;    // single value

    Tiles stages[4];        // 64^3, 16^3, 4^3, voxels

    Ptr<int32_t> num_active_tiles;  // GPU-allocated count of active tiles

    Ptr<void> values; // Used to pass data around
    size_t values_size=0;

    Ptr<uint32_t[]> normals;
};

} // mpr
