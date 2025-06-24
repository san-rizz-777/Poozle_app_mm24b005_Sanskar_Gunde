#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <regex>
#include <thread>
#include <mutex>
#include <chrono>
#include <optional>
#include <algorithm>

std::mutex result_mutex;

class RegexBuilder {
public:
    static std::regex build(std::string_view pattern, bool ignore_case = false) {
        constexpr std::string_view special_chars = "^$.*+?()[]{}|\\";
        auto escape_char = [](char c) -> std::string {
            return (special_chars.find(c) != std::string_view::npos)
                ? std::string("\\") + c
                : std::string(1, c);
        };

        std::string escaped_pattern;
        for (const auto& c : pattern) {
            escaped_pattern += escape_char(c);
        }

        using namespace std::string_literals;
        std::string regex_pattern = "^"s + escaped_pattern + "$"s +           // exact match
                                    "|^.*"s + escaped_pattern + "$"s +        // ends with
                                    "|^"s + escaped_pattern + ".*$"s +        // starts with
                                    "|^.*"s + escaped_pattern + ".*$"s;       // contains

        return std::regex(regex_pattern, ignore_case ? std::regex_constants::icase : std::regex_constants::ECMAScript);
    }
};

class SearchEngine {
public:
    static void search_chunk(const std::vector<std::string>& words, const std::regex& search_regex,
                             size_t start, size_t end, std::vector<std::string>* results,
                             std::atomic<size_t>* count, bool store_matches) {
        std::vector<std::string> local_results;

        for (size_t i = start; i < end; ++i) {
            if (std::regex_match(words[i], search_regex)) {
                if (store_matches) {
                    local_results.emplace_back(words[i]);
                } else {
                    ++(*count);
                }
            }
        }

        if (store_matches && results) {
            std::lock_guard<std::mutex> lock(result_mutex);
            results->insert(results->end(),
                            std::make_move_iterator(local_results.begin()),
                            std::make_move_iterator(local_results.end()));
        }
    }

    static std::optional<std::chrono::milliseconds> search(const std::vector<std::string>& words,
        std::string_view pattern,
        std::vector<std::string>* results,
        bool ignore_case = false,
        bool store_matches = true,
        std::optional<int> num_threads = std::nullopt,
        std::optional<std::atomic<size_t>*> match_count = std::nullopt) {

        if (words.empty()) return std::nullopt;

        auto start_time = std::chrono::high_resolution_clock::now();
        int threads_to_use = num_threads.value_or(std::min<int>(std::thread::hardware_concurrency(), 8));
        size_t chunk_size = words.size() / threads_to_use;

        std::vector<std::thread> threads;
        threads.reserve(threads_to_use);
        std::regex search_regex = RegexBuilder::build(pattern, ignore_case);

        for (int t = 0; t < threads_to_use; ++t) {
            size_t start = t * chunk_size;
            size_t end = (t == threads_to_use - 1) ? words.size() : start + chunk_size;

            threads.emplace_back(search_chunk, std::cref(words), std::cref(search_regex),
                                 start, end, results,
                                 match_count.value_or(nullptr), store_matches);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    }
};

bool validate_input(int n) {
    return n > 0 && n <= 1000000;
}

std::vector<std::string> load_words_from_file(const std::string& filepath) {
    std::ifstream infile(filepath);
    std::vector<std::string> words;
    std::string word;
    while (std::getline(infile, word)) {
        if (!word.empty()) words.emplace_back(std::move(word));
    }
    return words;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./search_tool <input_file.txt> <pattern> [--ignore-case] [--count-only] [--threads=N]\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string pattern = argv[2];
    bool ignore_case = false;
    bool count_only = false;
    int thread_count = 0;

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--ignore-case") ignore_case = true;
        else if (arg == "--count-only") count_only = true;
        else if (arg.rfind("--threads=", 0) == 0)
            thread_count = std::stoi(arg.substr(9));
    }

    auto words = load_words_from_file(filename);
    if (words.empty()) {
        std::cerr << "Failed to read words from file or file is empty.\n";
        return 1;
    }

    std::vector<std::string> matches;
    std::atomic<size_t> match_counter{0};
    auto timing = SearchEngine::search(
        words, pattern,
        count_only ? nullptr : &matches,
        ignore_case, !count_only,
        thread_count ? std::optional<int>{thread_count} : std::nullopt,
        count_only ? std::optional<std::atomic<size_t>*>(&match_counter) : std::nullopt
    );

    if (!timing) {
        std::cerr << "Search failed.\n";
        return 1;
    }

    std::cout << "Search completed in " << timing->count() << "ms\n";
    size_t match_count = count_only ? match_counter.load() : matches.size();
    std::cout << "Matches found: " << match_count << "\n";

    if (!count_only && !matches.empty()) {
        for (const auto& match : matches) {
            std::cout << match << '\n';
        }
    }

    // Optional: log to benchmark.csv
    std::ofstream log("benchmark.csv", std::ios::app);
    log << pattern << "," << match_count << "," << timing->count() << "ms\n";

    return 0;
}
