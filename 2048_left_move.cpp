#include <iostream>
#include <array>
#include <vector>
#include <string>

class RowHandler {
public:
    using Row = std::array<int, 4>;
    
    static Row moveLeft(const Row& input) {
        Row temp = input;
        
        // Stage 1: Compact non-zero to left
        int writeIdx = 0;
        for (int i = 0; i < 4; ++i) {
            if (temp[i] != 0) temp[writeIdx++] = temp[i];
        }
        while (writeIdx < 4) temp[writeIdx++] = 0;
        
        // Stage 2: Merge adjacent equals (skip next after merge)
        for (int i = 0; i < 3; ) {
            if (temp[i] != 0 && temp[i] == temp[i + 1]) {
                temp[i] *= 2;
                temp[i + 1] = 0;
                i += 2;  // Prevent chain merge
            } else {
                ++i;
            }
        }
        
        // Stage 3: Compact again
        Row result = {0, 0, 0, 0};
        writeIdx = 0;
        for (int i = 0; i < 4; ++i) {
            if (temp[i] != 0) result[writeIdx++] = temp[i];
        }
        return result;
    }
};

struct TestCase {
    RowHandler::Row input;
    RowHandler::Row expected;
    std::string description;
};

void runTests() {
    std::vector<TestCase> tests = {
        {{0, 0, 0, 0}, {0, 0, 0, 0}, "All zeros"},
        {{2, 4, 8, 16}, {2, 4, 8, 16}, "No zero, no merge"},
        {{0, 2, 0, 4}, {2, 4, 0, 0}, "Has zero, no merge after shift"},
        {{2, 2, 4, 8}, {4, 4, 8, 0}, "Front merge"},
        {{2, 4, 4, 8}, {2, 8, 8, 0}, "Middle merge"},
        {{2, 4, 8, 8}, {2, 4, 16, 0}, "Rear merge"},
        {{2, 0, 2, 4}, {4, 4, 0, 0}, "Zero gap then merge"},
        {{2, 2, 4, 4}, {4, 8, 0, 0}, "Two independent merges"},
        {{2, 2, 2, 2}, {4, 4, 0, 0}, "Four identical"},
        {{2, 2, 2, 4}, {4, 2, 4, 0}, "Three identical (prevent re-merge)"}
    };
    
    std::cout << "========== Test Suite ==========" << std::endl;
    bool allPassed = true;
    for (size_t i = 0; i < tests.size(); ++i) {
        auto result = RowHandler::moveLeft(tests[i].input);
        bool pass = (result == tests[i].expected);
        if (!pass) allPassed = false;
        
        std::cout << "Test " << (i + 1) << ": " << tests[i].description 
                  << " ... " << (pass ? "PASS" : "FAIL") << std::endl;
        if (!pass) {
            std::cout << "  Input:    ";
            for (auto x : tests[i].input) std::cout << x << " ";
            std::cout << "\n  Expected: ";
            for (auto x : tests[i].expected) std::cout << x << " ";
            std::cout << "\n  Got:      ";
            for (auto x : result) std::cout << x << " ";
            std::cout << std::endl;
        }
    }
    std::cout << "================================" << std::endl;
    std::cout << (allPassed ? "All tests passed!" : "Some tests failed!") << std::endl;
    std::cout << std::endl;
}

int main() {
    // Run all 10 test cases automatically
    runTests();
    
    // Interactive mode
    RowHandler::Row input;
    std::cout << "Enter 4 integers (space separated): ";
    std::cin >> input[0] >> input[1] >> input[2] >> input[3];
    
    auto result = RowHandler::moveLeft(input);
    
    std::cout << "Result: ";
    for (int i = 0; i < 4; ++i) {
        std::cout << result[i] << (i < 3 ? " " : "");
    }
    std::cout << std::endl;
    
    return 0;
}