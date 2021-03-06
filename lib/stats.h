#pragma once

#include <atomic>


class Stats {
public:
    static Stats& instance() {
        static Stats instance;
        return instance;
    }

    size_t num_triangles;
    size_t kdtree_height;
    std::atomic<size_t> num_rays;       // all rays
    std::atomic<size_t> num_prim_rays;  // primary rays
    size_t runtime_ms;
    size_t loading_time_ms;

private:
    Stats() {}
    Stats(const Stats&) = delete;
    Stats operator=(const Stats&) = delete;
};
