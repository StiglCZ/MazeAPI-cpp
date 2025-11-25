#pragma once
#include "numbers.hh"
#include <vector>

enum Materials {
    Wall,
    Space,
    Start,
    Finish,
};

extern std::vector<std::vector<u8>> CreateMaze(u16 Width, u16 Height, i32 Seed = 0);
