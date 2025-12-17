#include "maze.hh"

#include <cstdlib>
#include <chrono>
#include <stack>

using std::vector;
using std::stack;
using std::pair;

typedef pair<i16, i16> point;
typedef vector<vector<u8>> field;
bool IsUnoccupiedAndValid(const point& pos, const field& field, const bool Revisiting = false) {
    return
        (pos.first >= 0  && pos.first < field.size()) &&
        (pos.second >= 0 && pos.second < field[0].size()) &&
        (field[pos.first][pos.second] == Materials::Wall || Revisiting);
}

u64 GetUnixNanosecond() {
    // Chrono returns nanoseconds by default
    return std::chrono::system_clock::now().time_since_epoch().count();
}

point CenterPoint(const point& a, const point& b) {
    // a + half of difference = middlepoint inbetween a and b
    return {
        a.first + (b.first - a.first) / 2,
        a.second + (b.second - a.second) / 2,
    };
}

std::vector<point> CreateAllValid(const point& pos, const field& field, const bool Revisiting = false) {
    const std::vector<point> directions = {
        {pos.first, pos.second - 2},
        {pos.first, pos.second + 2},
        {pos.first - 2, pos.second},
        {pos.first + 2, pos.second}
    };

    std::vector<point> result;
    for(const point p : directions)
        if(IsUnoccupiedAndValid(p, field, Revisiting))
            result.push_back(p);
    return result;
}

void RemoveSnakeEye(field& field, const u16& Width, const u16& Height, const u32& x, const u32& y) {
    if(field[x][y] != Materials::Wall)
        return;

    const std::vector<point> neighbours = {
        { x + 1, y },
        { x - 1, y },
        { x, y + 1 },
        { x, y - 1 }
    };

    std::vector<point> fillableNeighbours;
    for(point p : neighbours) {
        const bool pastBorders = p.first < 0 || p.second < 0 || p.first >= Width || p.second >= Height;
        if(pastBorders) continue;

        const bool& isWall = field[p.first][p.second] == Materials::Wall;
        const bool& isSpace = field[p.first][p.second] == Materials::Space;
        if(isWall) // Neighbouring a wall, not a snakeeye
            return;
        if(isSpace) // Neighbouring a space, add to list
            fillableNeighbours.push_back(p);
    }

    if(fillableNeighbours.empty()) // Propably a 1x1 maze or something, let return
        return;
    point& selectedFillableNeighbour = fillableNeighbours[rand() % fillableNeighbours.size()];
    field[selectedFillableNeighbour.first][selectedFillableNeighbour.second] = Materials::Wall;
}

void RemoveSnakeEyes(const u16& Width, const u16& Height, field& field) {
    for(u32 x =0; x < Width; x++)
        for(u32 y =0; y < Height; y++)
            RemoveSnakeEye(field, Width, Height, x, y);
}

vector<vector<u8>> CreateMaze(u16 Width, u16 Height, i32 Seed, bool revisit) {
    if(Width == 0 || Height == 0)
        return {};

    bool CurrentlyRevisiting = false;
    field field(Width, vector<u8>(Height, Materials::Wall));
    stack<point> history;
    point position = {1, 1};
    field[1][1] = Materials::Start;

    std::srand((Seed)? Seed : GetUnixNanosecond());
    while(true) {
        if(revisit) CurrentlyRevisiting = (std::rand() % 10) > 8;

        const std::vector<point> ValidSteps = CreateAllValid(position, field, CurrentlyRevisiting);

        if(ValidSteps.empty()) {
            if(history.empty()) break;

            position = history.top();
            history.pop();
            continue;
        }

        history.push(position);
        position = ValidSteps[std::rand() % ValidSteps.size()];

        point midstep = CenterPoint(history.top(), position);
        field[midstep.first][midstep.second]   = Materials::Space;
        field[position.first][position.second] = Materials::Space;
    }

    field[field.size() -2][field[0].size() -2] = Materials::Finish;

    if(revisit)
        RemoveSnakeEyes(Width, Height, field);
    return field;
}
