#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <cstring>
#include <set>
#include <memory>
#include <cassert>
#include <queue>
#include <functional>
#include <thread>


struct Rnd {
    explicit Rnd(unsigned long x = 123456789,
                 unsigned long y = 362436069,
                 unsigned long z = 521288629)
        : x(x), y(y), z(z) {}

    int rand_int(unsigned int max) {
        unsigned int mask = max;
        mask |= mask >> 1;
        mask |= mask >> 2;
        mask |= mask >> 4;
        mask |= mask >> 8;
        mask |= mask >> 16;

        while(true) {
            auto t = xor_shf96() & mask;
            if(t < max)
                return (int)t;
        }
    }

private:
    unsigned long xor_shf96() {          //period 2^96-1
        unsigned long t;
        x ^= x << 16;
        x ^= x >> 5;
        x ^= x << 1;

        t = x;
        x = y;
        y = z;
        z = t ^ x ^ y;

        return z;
    }

    unsigned long x, y, z;
};


struct position: public std::pair<int, int> {
    using std::pair<int, int>::pair;

    int &x() {return std::pair<int, int>::first;};
    int &y() {return std::pair<int, int>::second;};
    [[nodiscard]] const int &x() const {return std::pair<int, int>::first;};
    [[nodiscard]] const int &y() const {return std::pair<int, int>::second;};

    [[nodiscard]] std::array<position, 4> neighbors() const {
        return {position{first - 1, second},
                position{first, second - 1},
                position{first + 1, second},
                position{first, second + 1}};
    }

    [[nodiscard]] int dist(const position &other) const {
        return abs(other.x() - x()) +
               abs(other.y() - y());
    }
};


class User_exception: public std::exception {
public:
    explicit User_exception(std::string msg) noexcept : err_msg(std::move(msg)) {};

    [[nodiscard]] const char *what() const noexcept override {
        return err_msg.c_str();
    }
protected:
    std::string err_msg;
};


class Not_a_file_error: public User_exception {
public:
    explicit Not_a_file_error(const char *filename) noexcept
        : User_exception(std::string{"File "} + filename + "; cannot be opened.")
    {}
};

class Bad_format: public User_exception {
public:
    explicit Bad_format(const std::string &msg) noexcept
        : User_exception(std::string{"Wrong format: "} + msg)
    {}
};

class BadAlgorithm: public User_exception {
public:
    explicit BadAlgorithm(const char *alg) noexcept
        : User_exception(std::string{"Algorithm not supported: "} + alg)
    {}
};


class Tile {
    const static uint8_t WALL   = 0b0000'0001,
                         OPEN   = 0b0000'0010,
                         CLOSED = 0b0000'0100,
                         START  = 0b0000'1000,
                         END    = 0b0001'0000,
                         PATH   = 0b0010'0000;
    uint8_t val;

    explicit Tile(uint8_t val) : val(val) {}

public:
    explicit Tile(char c) {
        if(c == 'X')      val = WALL;
        else if(c == ' ') val = 0;
        else throw Bad_format(std::string{"Char '"} + c + "'");
    }

    void set_start()           { val |= START; }
    void set_end()             { val |= END; }
    void set_path()            { val |= PATH; }
    void open()                { val |= OPEN; }
    void open_from(Tile &from) { val |= OPEN; previous = &from; }
    void close()               { val |= CLOSED; }

    [[nodiscard]] bool is_open() const   { return val & OPEN && !(val & CLOSED); }
    [[nodiscard]] bool is_closed() const { return val & CLOSED; }
    [[nodiscard]] bool is_empty() const  { return (val & (WALL | OPEN | CLOSED)) == 0; }
    [[nodiscard]] bool is_end() const    { return val & END; }

    [[nodiscard]] std::string repr() const {
        std::string res;
        if(val & PATH) res += "\u001b[31m";

        if(val & END)         res += "E";
        else if(val & START)  res += "S";
        else if(val & PATH)   res += "#";
        else if(val & CLOSED) res += "-";
        else if(val & OPEN)   res += "O";
        else if(val & WALL)   res += "\u001b[47m \u001b[0m";
        else if(val == 0)     res += " ";
        else res += "?";

        if(val & PATH)  res += "\u001b[0m";
        return res;
    }

    Tile *previous = nullptr;
};


struct Maze {
    explicit Maze(const char *filename) {
        std::ifstream maze_file{filename};
        if(!maze_file.is_open()) {
            throw Not_a_file_error{filename};
        }

        while(maze_file) {
            constexpr int BUFFER_SIZE = 4096;
            char buffer[BUFFER_SIZE];
            maze_file.getline(buffer, BUFFER_SIZE);

            if(buffer[0] == 's') {
                char *temp;
                long x = strtol(buffer + 6, &temp, 10);
                long y = strtol(temp + 2, &temp, 10);
                start = position{x, y};
            } else if(buffer[0] == 'e') {
                char *temp;
                long x = strtol(buffer + 4, &temp, 10);
                long y = strtol(temp + 2, &temp, 10);
                end = position{x, y};
                break;

            } else if(buffer[0] == 'X') {
                grid.emplace_back(parse_row(buffer));
            } else {
                throw Bad_format(std::string{"Line start: '"} + buffer[0] + "'");
            }
        }

        (*this)[start].set_start();
        (*this)[end].set_end();
    }

    Tile &operator[](const position &pos) { return grid[pos.y()][pos.x()]; }
    const Tile &operator[](const position &pos) const { return grid[pos.y()][pos.x()]; }

    [[nodiscard]] size_t width() const {
        return grid[0].size();
    }

    [[nodiscard]] size_t height() const {
        return grid.size();
    }

    std::vector<std::vector<Tile>> grid;
    position start;
    position end;
private:
    static std::vector<Tile> parse_row(const char *line) {
        std::vector<Tile> row;

        for(; *line; line++)
            row.emplace_back(*line);

        return row;
    }
};

std::ostream &operator<<(std::ostream &os, const Maze &maze) {
    os << "\033[2J\033[H";

    for(const auto &row: maze.grid) {
        for(const auto &elem: row)
            os << elem.repr();
        os << '\n';
    }
    os.flush();

    return os;
}


struct Search_Algorithm {
    explicit Search_Algorithm(Maze &maze)
        : maze(maze)
    {}

    virtual ~Search_Algorithm() = default;

    virtual void step() = 0;

    int backtrack_path() {
        assert(finished && path_exists);

        int dist = 0;
        Tile *temp = &maze[maze.end];
        temp->set_path();

        while(temp->previous) {
            temp = temp->previous;
            temp->set_path();
            dist++;
        }

        return dist;
    }

    Maze &maze;
    bool finished = false;
    bool path_exists = false;
protected:
    Rnd rnd_gen{};
};

struct RandomSearch: public Search_Algorithm {
    explicit RandomSearch(Maze &maze)
        : Search_Algorithm(maze)
    {
        open_positions.push_back(maze.start);
        maze[maze.start].open();
        if(maze[maze.end].is_open())
            finished = path_exists = true;
    }

    void step() override {
        if(finished) return;

        if(open_positions.empty()) {
            finished = true;
            return;
        }

        int pos_idx = rnd_gen.rand_int(open_positions.size());

        auto pos = open_positions[pos_idx];
        open_positions[pos_idx] = open_positions.back();
        open_positions.pop_back();
        auto &search_tile = maze[pos];

        for(const auto &ngb_pos: pos.neighbors()) {
            auto &ngb = maze[ngb_pos];

            if(ngb.is_empty()) {
                open_positions.push_back(ngb_pos);
                ngb.open_from(search_tile);
                if(ngb.is_end())
                    finished = path_exists = true;
            }
        }
        search_tile.close();
    }

    std::vector<position> open_positions;
};

struct DepthFirstSearch: public Search_Algorithm {
    explicit DepthFirstSearch(Maze &maze)
        : Search_Algorithm(maze)
    {
        open_positions.push_back(maze.start);
        maze[maze.start].open();
        if(maze[maze.end].is_open())
            finished = path_exists = true;
    }

    void step() override {
        if(finished) return;

        if(open_positions.empty()) {
            finished = true;
            return;
        }

        auto pos = open_positions.back();
        auto &search_tile = maze[pos];

        std::vector<int> ngb_idx{0, 1, 2, 3};
        auto &&neighbors = pos.neighbors();

        while(!ngb_idx.empty()) {
            auto idx_idx = rnd_gen.rand_int(ngb_idx.size());
            const auto &ngb_pos = neighbors[ngb_idx[idx_idx]];

            ngb_idx.erase(ngb_idx.begin() + idx_idx);
            auto &ngb = maze[ngb_pos];

            if(ngb.is_empty()) {
                open_positions.push_back(ngb_pos);
                ngb.open_from(search_tile);
                if(ngb.is_end())
                    finished = path_exists = true;
                return;
            }
        }

        open_positions.pop_back();
        search_tile.close();
    }

    std::vector<position> open_positions;
};

struct BreadthFirstSearch: public Search_Algorithm {
    explicit BreadthFirstSearch(Maze &maze)
        : Search_Algorithm(maze)
    {
        open_positions.push(maze.start);
        maze[maze.start].open();
        if(maze[maze.end].is_open())
            finished = path_exists = true;
    }

    void step() override {
        if(finished) return;

        if(open_positions.empty()) {
            finished = true;
            return;
        }

        auto pos = open_positions.front();
        auto &search_tile = maze[pos];

        std::vector<int> ngb_idx{0, 1, 2, 3};
        auto &&neighbors = pos.neighbors();

        while(!ngb_idx.empty()) {
            auto idx_idx = rnd_gen.rand_int(ngb_idx.size());
            const auto &ngb_pos = neighbors[ngb_idx[idx_idx]];

            ngb_idx.erase(ngb_idx.begin() + idx_idx);
            auto &ngb = maze[ngb_pos];

            if(ngb.is_empty()) {
                open_positions.push(ngb_pos);
                ngb.open_from(search_tile);
                if(ngb.is_end())
                    finished = path_exists = true;
            }
        }

        open_positions.pop();
        search_tile.close();
    }

    std::queue<position> open_positions;
};

struct GreedySearch: public Search_Algorithm {
    explicit GreedySearch(Maze &maze)
        : Search_Algorithm(maze),
          comp(
            [this](const position &a, const position &b) {
                return heuristic(a) > heuristic(b);
            }
          ),
          open_positions(comp)
    {
        open_positions.push(maze.start);
        maze[maze.start].open();
        if(maze[maze.end].is_open())
            finished = path_exists = true;
    }

    void step() override {
        if(finished) return;

        if(open_positions.empty()) {
            finished = true;
            return;
        }

        auto pos = open_positions.top();
        open_positions.pop();

        auto &search_tile = maze[pos];

        for(const auto &ngb_pos: pos.neighbors()) {
            auto &ngb = maze[ngb_pos];

            if(ngb.is_empty()) {
                open_positions.push(ngb_pos);
                ngb.open_from(search_tile);
                if(ngb.is_end())
                    finished = path_exists = true;
            }
        }
        search_tile.close();
    }

    std::function<bool(const position &, const position &)> comp;
    std::priority_queue<position,
                        std::vector<position>,
                        decltype(comp)> open_positions;

    [[nodiscard]] int heuristic(const position &p) const {
        return maze.end.dist(p);
    }
};

struct AStartSearch: public Search_Algorithm {
    explicit AStartSearch(Maze &maze)
        : Search_Algorithm(maze),
          comp(
              [this](const position &a, const position &b) {
                  return heuristic(a) > heuristic(b);
              }
          ),
          open_positions(comp),
          optimal_dist(maze.height(), std::vector(maze.width(), INT32_MAX))
    {
        open_positions.push(maze.start);
        maze[maze.start].open();
        optimal_dist[maze.start.y()][maze.start.x()] = 0;
        if(maze[maze.end].is_open())
            finished = path_exists = true;
    }

    void step() override {
        if(finished) return;

        if(open_positions.empty()) {
            finished = true;
            return;
        }

        while(maze[open_positions.top()].is_closed())
            open_positions.pop();

        auto pos = open_positions.top();
        open_positions.pop();

        auto &search_tile = maze[pos];

        for(const auto &ngb_pos: pos.neighbors()) {
            auto &ngb = maze[ngb_pos];

            if((ngb.is_open() || ngb.is_empty()) &&
               optimal_dist[ngb_pos.y()][ngb_pos.x()] > optimal_dist[pos.y()][pos.x()] + 1) {
                optimal_dist[ngb_pos.y()][ngb_pos.x()] = optimal_dist[pos.y()][pos.x()] + 1;
                ngb.previous = &search_tile;
                open_positions.push(ngb_pos);
            }

            if(ngb.is_empty()) {
                ngb.open_from(search_tile);
                if(ngb.is_end())
                    finished = path_exists = true;
            }

        }
        search_tile.close();
    }

    std::function<bool(const position &, const position &)> comp;
    std::priority_queue<position,
            std::vector<position>,
            decltype(comp)> open_positions;
    std::vector<std::vector<int>> optimal_dist;

    [[nodiscard]] int heuristic(const position &p) const {
        return maze.end.dist(p) + optimal_dist[p.y()][p.x()];
    }
};

std::unique_ptr<Search_Algorithm> algorithm_factory(Maze &maze, const char *mode) {
    if(!strcasecmp(mode, "random"))
        return std::make_unique<RandomSearch>(maze);
    if(!strcasecmp(mode, "dfs"))
        return std::make_unique<DepthFirstSearch>(maze);
    if(!strcasecmp(mode, "bfs"))
        return std::make_unique<BreadthFirstSearch>(maze);
    if(!strcasecmp(mode, "greedy"))
        return std::make_unique<GreedySearch>(maze);
    if(!strcasecmp(mode, "a") || !strcasecmp(mode, "star") || !strcasecmp(mode, "astar"))
        return std::make_unique<AStartSearch>(maze);

    throw BadAlgorithm(mode);
}

struct Input {
    static bool get_command() {
        constexpr int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        std::cin.getline(buffer, BUFFER_SIZE);

        return !(!strcmp(buffer, "c") ||
                 !strcmp(buffer, "cont") ||
                 !strcmp(buffer, "continue"));
    }
};



int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cerr << "Missing file, usage: " << argv[0] << " file mode [-OPTIONS]" << std::endl;
        return 1;
    }
    if(argc < 3) {
        std::cerr << "Missing mode, usage: " << argv[0] << " file mode [-OPTIONS]" << std::endl;
        return 1;
    }

    char *filename = argv[1];
    const char *mode = argv[2];
    bool step_flag = false;
    long frames = 1;
    long sleep_t = 20;

    if(argc > 3) {
        for(int c = 3; c < argc; c++) {
            if(!strcmp(argv[c], "--step")) {
                step_flag = true;
            }

            if(!strcmp(argv[c], "--frames") && c + 1 < argc) {
                frames = strtol(argv[c+1], nullptr, 10);
            }

            if(!strcmp(argv[c], "--sleep") && c + 1 < argc) {
                sleep_t = strtol(argv[c+1], nullptr, 10);
            }
        }
    }

    try {
        Maze maze(filename);
        std::cout << maze;
        int steps = 0;

        auto alg = algorithm_factory(maze, mode);
        while(!alg->finished) {
            if(step_flag) {
                step_flag = Input::get_command();
            }

            alg->step(); steps++;

            if(steps > frames || step_flag) {
                std::cout << maze;
                steps = 0;
                if(!step_flag)
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_t));
            }
        }

        if(alg->path_exists) {
            int dist = alg->backtrack_path();
            std::cout << maze;
            std::cout << "Path length: " << dist << std::endl;
        } else {
            std::cout << "No path exists." << std::endl;
        }

    }
    catch(User_exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
