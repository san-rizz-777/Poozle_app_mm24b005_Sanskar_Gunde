#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <thread>
#include <algorithm>

// Regex patterns to match valid instructions
const std::regex token_pattern(R"(do_add\(\)|don't_add\(\)|do_mul\(\)|don't_mul\(\)|add\(\d+,\d+\)|mul\(\d+,\d+\))");
const std::regex add_pattern(R"(add\((\d+),(\d+)\))");
const std::regex mul_pattern(R"(mul\((\d+),(\d+)\))");

// Extract valid tokens from each line using regex, within the specified line range [start, end)
void extractTokensPerLine(const std::vector<std::string>& lines,
                          std::vector<std::vector<std::string>>& token_lines,
                          int start, int end) {
    for (int i = start; i < end; ++i) {
        for (auto it = std::sregex_iterator(lines[i].begin(), lines[i].end(), token_pattern);
             it != std::sregex_iterator(); ++it) {
            token_lines[i].emplace_back(it->str());
        }
    }
}

int main() {
    std::cin.tie(nullptr);  // Untie cin from cout for faster I/O
    int n;
    std::cin >> n;
    std::cin.ignore();  // Ignore the newline after the integer input

    // Read all input lines
    std::vector<std::string> lines(n);
    for (auto& line : lines)
        std::getline(std::cin, line);

    // Each line will have its own list of extracted tokens
    std::vector<std::vector<std::string>> token_lines(n);

    // Determine number of threads (up to 4 or number of lines, whichever is smaller)
    int num_threads = std::min(4, n);
    int chunk_size = (n + num_threads - 1) / num_threads;
    std::vector<std::thread> threads;

    // Launch threads to extract tokens from chunks of lines
    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunk_size;
        int end = std::min(start + chunk_size, n);
        threads.emplace_back(extractTokensPerLine, std::cref(lines), std::ref(token_lines), start, end);
    }

    // Wait for all threads to complete
    for (auto& t : threads)
        t.join();

    // Initial state: both addition and multiplication enabled, result starts at 0
    bool add_enabled = true, mul_enabled = true;
    long long result = 0;

    // Process each token in order, updating flags and computing result
    for (const auto& tokens : token_lines) {
        for (const auto& token : tokens) {
            std::smatch match;
            if (token == "do_add()") {
                add_enabled = true;
            } else if (token == "don't_add()") {
                add_enabled = false;
            } else if (token == "do_mul()") {
                mul_enabled = true;
            } else if (token == "don't_mul()") {
                mul_enabled = false;
            } else if (add_enabled && std::regex_match(token, match, add_pattern)) {
                // If addition is enabled and token is add(x, y), add x + y to result
                result += std::stoll(match[1]) + std::stoll(match[2]);
            } else if (mul_enabled && std::regex_match(token, match, mul_pattern)) {
                // If multiplication is enabled and token is mul(x, y), add x * y to result
                result += std::stoll(match[1]) * std::stoll(match[2]);
            }
        }
    }

    // Print the final computed result
    std::cout << result << '\n';
    return 0;
}

