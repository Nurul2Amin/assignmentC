#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <unistd.h> // For sleep
#include <cctype>   // For toupper, tolower
#include <cstdlib>  // For rand

std::mutex mu;

char slow_tolower(char ch) {
    ch = tolower(ch);
    sleep((rand() % 2) + 1);  // Sleep for 1 or 2 seconds randomly
    return ch;
}

char slow_toupper(char ch) {
    ch = toupper(ch);
    sleep((rand() % 3) + 1);  // Sleep for 1 to 3 seconds randomly
    return ch;
}

void process_segment(const std::string& input, int start, int end, std::vector<char>& result, int id) {
    for (int i = start; i < end; ++i) {
        char processed_char;
        if (std::isupper(input[i])) {
            processed_char = slow_tolower(input[i]);
        } else {
            processed_char = slow_toupper(input[i]);
        }
        
        // Lock mutex to print and update result safely
        std::lock_guard<std::mutex> guard(mu);
        std::cout << "Thread-" << id << ": " << input[i] << " -> " << processed_char << std::endl;
        result[i] = processed_char;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <string>" << std::endl;
        return 1;
    }

    std::string input = argv[1];
    std::vector<char> result(input.size());
    std::vector<std::thread> threads;
    int num_threads = 4;

    // Split input into parts and create threads
    int part = input.size() / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        int start = i * part;
        int end = (i == num_threads - 1) ? input.size() : (i + 1) * part;
        threads.emplace_back(process_segment, std::ref(input), start, end, std::ref(result), i + 1);
    }

    // Wait for all threads to finish
    for (auto& th : threads) {
        th.join();
    }

    // Output the result
    std::lock_guard<std::mutex> guard(mu);
    std::cout << "Answer: ";
    for (char ch : result) {
        std::cout << ch;
    }
    std::cout << std::endl;

    return 0;
}
