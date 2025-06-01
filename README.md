# Poozle_app_mm24b005_Sanskar_Gunde
This is a public repository for the "poozle" app codes and their explanation. 

#Question 1 - "Exact Searching" 
# Multithreaded String Search

A high-performance, multithreaded C++ application for searching strings using regex patterns with support for exact match, starts with, ends with, and contains operations.

## Features

- **Multithreaded Processing**: Utilizes up to 4 threads for parallel string searching
- **Multiple Search Types**: Supports exact match, starts with, ends with, and contains patterns
- **Modern C++**: Built with C++14 and C++17 features for optimal performance
- **Regex Pattern Matching**: Uses compiled regex for efficient string matching
- **Performance Timing**: Measures and reports search execution time
- **Safe Concurrency**: Thread-safe operations with mutex protection

## Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Standard library support for:
  - `<thread>`
  - `<mutex>` 
  - `<regex>`
  - `<string_view>`
  - `<optional>`
  - `<chrono>`

## Compilation

### Using g++
```bash
g++ -std=c++17 -O2 -pthread -o string_search main.cpp
```


## Usage

1. **Run the program**:
   ```bash
   ./string_search
   ```

2. **Input format**:
   - Enter the number of words
   - Enter each word on a separate line
   - Enter the search pattern


## Search Types

The program automatically searches for patterns in four ways:

1. **Exact Match**: `pattern` matches exactly
2. **Starts With**: `pattern*` matches strings starting with pattern
3. **Ends With**: `*pattern` matches strings ending with pattern  
4. **Contains**: `*pattern*` matches strings containing pattern

## Performance Features

- **Parallel Processing**: Divides work among multiple threads
- **Move Semantics**: Efficient memory management with C++14/17 features
- **Reserved Memory**: Pre-allocates vectors for optimal performance
- **Regex Compilation**: Compiles regex once and reuses across threads

## Technical Details

### Modern C++ Features Used

- **C++14**: Generic lambdas, auto return types, string literals, move iterators
- **C++17**: `string_view`, `optional`, CTAD, if-with-initializer
- **Thread Safety**: Mutex-protected shared data access
- **RAII**: Automatic resource management

### Architecture

```
main() 
├── Input validation and collection
├── exact_string_search()
│   ├── build_search_regex() - Creates compiled regex pattern
│   ├── Thread creation and work distribution
│   └── regex_search_chunk() - Per-thread search function
└── Results display with timing
```

## Error Handling

- Input validation for reasonable limits (1-1,000,000 words)
- Empty input detection
- Thread-safe error propagation
- Graceful handling of regex compilation errors

-----------------------------------------------------------------------------------------------------------------------------------------------------------------








