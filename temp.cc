#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include <numeric>
#include <variant>
#include <cstdint>

// ROX Runtime
using num32 = int32_t;
using num = int64_t;
using rox_float = double;
using rox_char = char;
using rox_bool = bool;
struct None { bool operator==(const None&) const { return true; } };
const None none = {};
template<typename T>
struct rox_result {
    T value;
    num32 err;
};
template<typename T>
bool isOk(rox_result<T> r) {
    return r.err == 0;
}
template<typename T>
T getValue(rox_result<T> r) {
    if (r.err != 0) {
        std::cerr << "Called getValue on runtime error result!" << std::endl;
        exit(r.err);
    }
    return r.value;
}
void print_loop(num32 n) {
    for (int i = 0; i < n; ++i) {
        std::cout << "Hello, World!" << std::endl;
    }
}
// Result constructors
template<typename T>
rox_result<T> ok(T value) { return {value, 0}; }
template<typename T>
rox_result<T> error(num32 code) { return {T{}, code}; }
const double pi = 3.141592653589793;
const double e  = 2.718281828459045;
class RoxString {
public:
    std::string val;
    RoxString(const char* s) : val(s) {}
    RoxString(std::string s) : val(std::move(s)) {}
    RoxString() = default;

    num size() const { return (num)val.size(); }
    bool operator==(const RoxString& other) const { return val == other.val; }
    bool operator!=(const RoxString& other) const { return val != other.val; }
};

std::ostream& operator<<(std::ostream& os, const RoxString& s) {
    return os << s.val;
}

RoxString rox_str(const char* s) {
    return RoxString(s);
}

// I/O
std::ostream& operator<<(std::ostream& os, const std::vector<char>& s) {
    for (char c : s) os << c;
    return os;
}

template<typename... Args>
None print(const Args&... args) {
    ((std::cout << args), ...);
    return none;
}


// List access
template<typename T>
rox_result<T> rox_at(const std::vector<T>& xs, num i) {
    if (i < 0 || i >= (num)xs.size()) return error<T>(1); // index_out_of_range
    return ok(xs[i]);
}

// String access
rox_result<char> rox_at(const RoxString& s, num i) {
    if (i < 0 || i >= s.size()) return error<char>(1);
    return ok(s.val[i]);
}

// String to List
std::vector<char> rox_to_list(const RoxString& s) {
    return std::vector<char>(s.val.begin(), s.val.end());
}

// Division
template<typename T>
rox_result<T> rox_div(T a, T b) {
    if (b == 0) return error<T>(3); // division_by_zero
    return ok(a / b);
}

// Modulo
template<typename T>
rox_result<T> rox_mod(T a, T b) {
    if (b == 0) return error<T>(3); // division_by_zero
    return ok(a % b);
}

num32 num32_abs(num32 x) { return std::abs(x); }
std::vector<char> numToString(num n) {
    std::string s = std::to_string(n);
    return std::vector<char>(s.begin(), s.end());
}

std::vector<char> charToString(char c) {
    return {c};
}

std::string any_to_string(num n) { return std::to_string(n); }
std::string any_to_string(char c) { return std::string(1, c); }
std::string any_to_string(bool b) { return b ? "true" : "false"; }

template <typename T>
std::vector<char> listToString(const std::vector<T>& list) {
    std::string s = "[";
    for (size_t i = 0; i < list.size(); ++i) {
        if (i > 0) s += ", ";
        s += any_to_string(list[i]);
    }
    s += "]";
    return std::vector<char>(s.begin(), s.end());
}


// End Runtime

int main() {
  print(std::vector{'H', 'e', 'l', 'l', 'o', ' ', 'R', 'O', 'X', '!', '\n'});
  num sum = ((num)0);
  for (auto i = ((num)0); i < ((num)10); i += ((num)1))   {
    (sum = (sum + i));
  }
  if ((sum == ((num)45)))   {
    print(rox_str("Sum is: "), rox_str("\n"), numToString(sum));
  }
  return 0;
}
