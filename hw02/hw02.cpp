#include <vector>
#include <iostream>
#include <set>
#include <limits>
#include <cmath>
#include <cstring>


//bool is_ok(const std::vector<size_t> &v) {
//    std::set<size_t> s;
//
//    for(const auto &e: v)
//        s.insert(e);
//
//    return v.size() == s.size();
//}


template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    os << '{';

    if(!vec.empty())
        os << vec[0];

    for(size_t idx = 1; idx < vec.size(); idx++)
        os << ", " << vec[idx];

    return os << '}';
}


struct Rnd {
    explicit Rnd(unsigned long x = 123456789,
                 unsigned long y = 362436069,
                 unsigned long z = 521288629)
            : x(x), y(y), z(z) {}

    unsigned int rand_int(unsigned int max) {
        unsigned int mask = max;
        mask |= mask >> 1;
        mask |= mask >> 2;
        mask |= mask >> 4;
        mask |= mask >> 8;
        mask |= mask >> 16;

        while(true) {
            auto t = xor_shf96() & mask;
            if(t < max)
                return t;
        }
    }

    template<typename T>
    void shuffle(std::vector<T> &vec) {
        size_t N = vec.size();
        for(size_t i = N - 1; i; i--) {
            std::swap(vec[i], vec[rand_int(i)]);
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

Rnd global_rnd;


struct TupleGen {
    explicit TupleGen(size_t N, size_t k)
        : N(N), k(k), state(k)
    {
        if(k > N)
            throw std::exception();

        for(size_t i = 0; i < k; i++)
            state[i] = i;
    }

    std::vector<size_t> next() {
        auto current = state;

        for(size_t idx = 1; idx <= k; idx++) {
            if(state[k - idx] < N - idx) {
                state[k - idx]++;
                idx--;
                for(; idx >= 1; idx--)
                    state[k - idx] = state[k - idx - 1] + 1;
                return current;
            }
        }

        return {};
    }

    size_t N, k;
private:
    std::vector<size_t> state;
};


template<typename T>
std::vector<std::vector<T>> permutations(const std::vector<T> &v) {
    if(v.size() <= 1)
        return {v};

    std::vector<std::vector<T>> res;
    for(int i = 0; i < v.size(); i++) {
        T last_em = v[i];
        auto temp = v;
        std::swap(temp[i], temp.back());
        temp.pop_back();

        auto &&perm = permutations(temp);
        for(auto &&p: perm){
            p.push_back(last_em);
            res.emplace_back(p);
        }
    }

    return res;
}


std::vector<std::vector<size_t>> permutations(size_t N) {
    std::vector<size_t> v(N);
    for(size_t i = 0; i < N; i++)
        v[i] = i;
    return permutations(v);
}


template<typename T>
void rotate(std::vector<T> &v, const std::vector<size_t> &idc) {
    if(idc.size() < 2) return;

    T temp = v[idc[0]];
    for(size_t idx = 1; idx < idc.size(); idx++)
        v[idc[idx - 1]] = v[idc[idx]];
    v[idc.back()] = temp;
}


struct State {
    State() = default;
    explicit State(std::vector<size_t> state)
            : state(std::move(state)),
              cost(_cost_function())
    {}

    explicit State(size_t N)
        : state(N)
    {
        for(size_t i = 0; i < N; i++)
            state[i] = i;

        global_rnd.shuffle(state);

        cost = _cost_function();
    }

    [[nodiscard]] std::vector<State> surrounding_roll(size_t k = 2) const {
        if(k <= 1)
            throw std::exception();

        std::vector<State> ngb;
        auto g = TupleGen(N(), k);
        std::vector<size_t> rot_idx;
        auto state_copy = state;

        while(!(rot_idx = g.next()).empty()) {
            for(size_t rn = 0; rn < k - 1; rn++) {
                rotate(state_copy, rot_idx);
                ngb.emplace_back(state_copy);
            }
            rotate(state_copy, rot_idx);
        }

        return ngb;
    }

    [[nodiscard]] std::vector<State> surrounding_perm_close(size_t k = 2) const {
        if(k <= 1)
            return {State{state}};


        std::vector<State> ngb;
        auto perms = permutations(k);
        perms.pop_back();

        for(size_t i = 0; i < N() - k; i++) {
            for(const auto &perm: perms) {
                auto state_copy = state;
                for(size_t ip = 0; ip < k; ip++)
                    state_copy[i + ip] = state[i + perm[ip]];

                ngb.emplace_back(state_copy);
            }
        }

        return ngb;
    }

    [[nodiscard]] size_t N() const {
        return state.size();
    }

    std::vector<size_t> state;
    double cost = std::numeric_limits<double>::infinity();
private:
    [[nodiscard]] double _cost_function() const {
        std::vector<size_t> diag1(2 * N());
        std::vector<size_t> diag2(2 * N());

        for(size_t c = 0; c < N(); c++) {
            diag1[c + state[c]]++;
            diag2[N() + c - state[c]]++;
        }

        double c = 0;
        for(const auto &e: diag1)
            if(e > 1)
                c += std::pow(2., e - 1) - 1;

        for(const auto &e: diag2)
            if(e > 1)
                c += std::pow(2., e - 1) - 1;
        return c;
    };
};


State hill_climb(const State &initial_state, bool roll) {
    std::cout << "Initial cost: " << initial_state.cost << std::endl;
    State current_state = initial_state;

    size_t k = 2;

    auto ngb_f = roll ? &State::surrounding_roll : &State::surrounding_perm_close;

    while(current_state.cost > 0.5) {
        State best_ngb = current_state;

        for(const auto &ngb: (current_state.*ngb_f)(k)) {
            if(ngb.cost < best_ngb.cost) {
                best_ngb = ngb;
            }
        }

        if(current_state.cost == best_ngb.cost) {
            if(k == current_state.N())
                return {};
            k++;
            std::cout << "New k = " << k << std::endl;
        }
        else {
            current_state = best_ngb;
            std::cout << "New cost: " << current_state.cost << std::endl;
        }
    }

    return current_state;
}


int usage(char *prog) {
    std::cerr << "Usage: " << prog << " N [{roll|perm}]" << std::endl;
    return 1;
}


int main(int argc, char *argv[]) {
    if(argc != 2 && argc != 3) {
        return usage(argv[0]);
    }

    bool roll = true;
    if(argc == 3) {
        if(!strcmp(argv[2], "perm")) roll = false;
        else if(strcmp(argv[2], "roll") != 0) return usage(argv[0]);
    }


    auto final_state = hill_climb(State(strtol(argv[1], nullptr, 10)), roll);

    if(final_state.state.empty())
        std::cout << "No solution found." << std::endl;
    else {
        std::string command{"./hw02/viz.py "};

        command += std::to_string(final_state.state[0]);

        for(size_t idx = 1; idx < final_state.state.size(); idx++) {
            command += ',';
            command += std::to_string(final_state.state[idx]);
        }
        std::system(command.c_str());
    }
}
