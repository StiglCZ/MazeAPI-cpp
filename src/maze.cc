#include "maze.hh"

#include <random>
#include <chrono>
#include <stack>

using std::vector;
using std::stack;
using std::pair;

typedef pair<i16, i16> point;
typedef vector<vector<u8>> field;
bool IsUnoccupiedAndValid(const point& pos, const field& field) {
    return
        pos.first >= 0  && pos.first < field.size() &&
        pos.second >= 0 && pos.second < field[0].size() &&
        field[pos.first][pos.second] == Materials::Wall;
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

std::vector<point> CreateAllValid(const point& pos, const field& field) {
    const std::vector<point> directions = {
        {pos.first, pos.second - 2},
        {pos.first, pos.second + 2},
        {pos.first - 2, pos.second},
        {pos.first + 2, pos.second}
    };

    std::vector<point> result;
    for(const point p : directions)
        if(IsUnoccupiedAndValid(p, field))
            result.push_back(p);
    return result;
}

vector<vector<u8>> CreateMaze(u16 Width, u16 Height, i32 Seed) {
    if(Width == 0 || Height == 0)
        return {};

    field field(Width, vector<u8>(Height, Materials::Wall));
    stack<point> history;
    point position = {1, 1};
    field[1][1] = Materials::Start;

    srand((Seed)? Seed : GetUnixNanosecond());
    while(true) {
        const std::vector<point> ValidSteps = CreateAllValid(position, field);

        if(ValidSteps.empty()) {
            if(history.empty()) break;

            position = history.top();
            history.pop();
            continue;
        }

        history.push(position);
        position = ValidSteps[rand() % ValidSteps.size()];

        point midstep = CenterPoint(history.top(), position);
        field[midstep.first][midstep.second]   = Materials::Space;
        field[position.first][position.second] = Materials::Space;
    }

    field[field.size() -2][field[0].size() -2] = Materials::Finish;
    return field;
}
