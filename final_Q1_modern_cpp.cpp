#include <iostream>  // For standard input output
#include <vector>    // For storing strings in vector
#include <string>    // For string manipulations
#include <thread>    // For thread creation
#include <mutex>     // For avoiding data racing and overwriting of vector
#include <regex>     // For regex searching
#include <algorithm> // For algorithms
#include <string_view> // C++17 string_view for efficient string handling
#include <optional>  // C++17 optional
#include <chrono>    // For timing

std::mutex result_mutex;

// C++14: Generic lambda with auto parameters
// C++17: constexpr if for compile-time branching
constexpr std::string_view special_chars = "^$.*+?()[]{}|\\";

// C++17: structured binding and string_view for better performance
std::regex build_search_regex(std::string_view pattern) {
    // C++14: using generic lambda with auto
    auto escape_char = [](char c) -> std::string {
        if (special_chars.find(c) != std::string_view::npos) {
            return std::string("\\") + c;
        }
        return std::string(1, c);
    };
    
    std::string escaped_pattern;
    // C++17: range-based for with structured binding would be ideal, but for chars we use traditional
    for (const auto& c : pattern) {
        escaped_pattern += escape_char(c);
    }
    
    // C++14: using string literal operator and better string concatenation
    using namespace std::string_literals;
    auto regex_pattern = "^"s + escaped_pattern + "$"s +           // exact match
                        "|^.*"s + escaped_pattern + "$"s +         // ends with
                        "|^"s + escaped_pattern + ".*$"s +         // starts with
                        "|^.*"s + escaped_pattern + ".*$"s;        // contains
    
    return std::regex(regex_pattern);
}

// C++17: using std::string_view for better performance
void regex_search_chunk(const std::vector<std::string>& words,
                        const std::regex& search_regex,
                        size_t start, size_t end,
                        std::vector<std::string>& results) {
    std::vector<std::string> local_results;
    
    // C++17: structured binding for cleaner iteration (if we had pairs)
    // Here we use range-based for with modern approach
    for (auto i = start; i < end; ++i) {
        if (std::regex_match(words[i], search_regex)) {
            local_results.emplace_back(words[i]);  // C++14: emplace_back instead of push_back
        }
    }

    // C++17: lock_guard with template argument deduction
    std::lock_guard lock{result_mutex};  // CTAD - Class Template Argument Deduction
    
    // C++14: using move semantics for better performance
    results.insert(results.end(), 
                   std::make_move_iterator(local_results.begin()), 
                   std::make_move_iterator(local_results.end()));
}

// C++14: auto return type deduction
auto calculate_optimal_threads(size_t data_size) -> int {
    // Default to 4 threads or data_size if smaller
    return std::min(4, static_cast<int>(data_size));
}

// C++17: optional return type for error handling
std::optional<std::chrono::milliseconds> exact_string_search(
    const std::vector<std::string>& words,
    std::string_view pattern,
    std::vector<std::string>& results,
    std::optional<int> num_threads = std::nullopt) {
    
    if (words.empty()) {
        return std::nullopt;  // C++17: return nullopt for empty input
    }
    
    // C++17: using auto with structured binding for time measurement
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    // C++14: auto type deduction with initialization
    const auto actual_threads = num_threads.value_or(calculate_optimal_threads(words.size()));
    
    const auto chunk_size = words.size() / actual_threads;
    std::vector<std::thread> threads;
    threads.reserve(actual_threads);  // C++14: reserve for better performance
    
    const auto search_regex = build_search_regex(pattern);

    // C++14: generic lambda for thread creation
    auto create_thread = [&](int t) {
        const auto start = t * chunk_size;
        const auto end = (t == actual_threads - 1) ? words.size() : start + chunk_size;
        return std::thread{regex_search_chunk, std::cref(words), 
                          std::cref(search_regex), start, end, std::ref(results)};
    };

    // C++14: range-based for with auto
    for (auto t = 0; t < actual_threads; ++t) {
        threads.emplace_back(create_thread(t));
    }

    // C++17: range-based for with auto and reference
    for (auto& thread : threads) {
        thread.join();
    }
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    return duration;  // C++17: return optional with timing info
}

// C++14: generic lambda for input validation
auto validate_input = [](int n) -> bool {
    return n > 0 && n <= 1000000;  // reasonable limits
};

int main() {
    // C++14: using auto for type deduction
    auto word_list = std::vector<std::string>{};
    auto n = 0;
    
    std::cout << "Enter number of words: ";
    std::cin >> n;
    
    if (!validate_input(n)) {
        std::cout << "Invalid input. Please enter a number between 1 and 1,000,000.\n";
        return 1;
    }
    
    std::cin.ignore();
    word_list.reserve(n);  // C++14: reserve capacity for better performance
    
    std::cout << "Enter " << n << " words:\n";
    
    // C++14: range-based for with auto
    for (auto i = 0; i < n; ++i) {
        auto word = std::string{};
        std::getline(std::cin, word);
        word_list.emplace_back(std::move(word));  // C++14: move semantics
    }
    
    auto pattern = std::string{};
    std::cout << "Enter search pattern: ";
    std::getline(std::cin, pattern);

    auto results = std::vector<std::string>{};
    
    // C++17: if with initializer and optional handling
    if (const auto timing = exact_string_search(word_list, pattern, results); timing) {
        std::cout << "\nSearch completed in " << timing->count() << "ms\n";
        std::cout << "Matches found (" << results.size() << "):\n";
        
        if (results.empty()) {
            std::cout << "No matches found.\n";
        } else {
            // C++17: range-based for with const auto&
            for (const auto& word : results) {
                std::cout << word << '\n';
            }
        }
    } else {
        std::cout << "Search failed - empty input.\n";
        return 1;
    }

    return 0;
}S