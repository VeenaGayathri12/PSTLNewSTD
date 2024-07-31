

#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <numeric>
#include <vector>

template <typename TFunc>
void RunAndMeasure(const char* title, TFunc func) {
    const auto start = std::chrono::steady_clock::now();
    auto ret = func();
    const auto end = std::chrono::steady_clock::now();
    std::cout << title << ": "
              << std::chrono::duration<double, std::milli>(end - start).count()
              << " ms, res " << ret << "\n";
}

template <typename ExecutionPolicy>
void RunTests(ExecutionPolicy&& policy) {
    int size = 1024000000;
    std::vector<double> v(size, 0.5);
    std::vector<double> result(v.size());

    std::vector<double> v1(size);
    std::iota(v1.begin(), v1.end(), 1.0);

    RunAndMeasure("std::accumulate", [&v] {
        return std::accumulate(v.begin(), v.end(), 0.0);
    });

    RunAndMeasure("std::reduce", [&v, &policy] {
        return std::reduce(policy, v.begin(), v.end(), 0.0);
    });

    RunAndMeasure("std::find", [&v, &policy] {
        auto res = std::find(policy, std::begin(v), std::end(v), 0.6);
        return res == std::end(v) ? 0.0 : 1.0;
    });

    RunAndMeasure("std::copy_if", [&v, &result, &policy] {
        auto new_end = std::copy_if(policy, v.begin(), v.end(), result.begin(),
                                    [](double value) { return value > 0.4; });
        return std::distance(result.begin(), new_end);
    });

    RunAndMeasure("std::inclusive_scan", [&v, &policy] {
        std::vector<double> scan_result(v.size());
        std::inclusive_scan(policy, v.begin(), v.end(), scan_result.begin());
        return scan_result.back();
    });

    RunAndMeasure("std::min_element", [&v1, &policy] {
        return *std::min_element(policy, v1.begin(), v1.end());
    });

    RunAndMeasure("std::max_element", [&v1, &policy] {
        return *std::max_element(policy, v1.begin(), v1.end());
    });

    RunAndMeasure("std::minmax_element", [&v1, &policy] {
        auto result = std::minmax_element(policy, v1.begin(), v1.end());
        return *result.first + *result.second;
    });

    RunAndMeasure("std::is_partitioned", [&v, &policy] {
        return std::is_partitioned(policy, v.begin(), v.end(), [](double n) { return n < 1.0; });
    });

    RunAndMeasure("std::lexicographical_compare", [&v, &policy] {
        std::vector<double> v2(1024000000, 0.5);
        return std::lexicographical_compare(policy, v.begin(), v.end(), v2.begin(), v2.end());
    });

    RunAndMeasure("std::binary_search", [&v] {
        return std::binary_search(v.begin(), v.end(), 0.5);
    });

    RunAndMeasure("std::lower_bound", [&v1] {
        return *std::lower_bound(v1.begin(), v1.end(), 0.5);
    });

    RunAndMeasure("std::upper_bound", [&v1] {
        return *std::upper_bound(v1.begin(), v1.end(), 0.5);
    });
}

int main() {
    char policy_choice;
    
    std::cout << "Choose execution policy: \n"
              << "1. Sequential\n"
              << "2. Parallel\n"
              << "3. Parallel Unsequenced\n"
	      << "Enter choice (1/2/3): ";
    std::cin >> policy_choice;

    switch (policy_choice) {
        case '1':
            std::cout << "Running with std::execution::seq\n";
            RunTests(std::execution::seq);
            break;
        case '2':
            std::cout << "Running with std::execution::par\n";
            RunTests(std::execution::par);
            break;
        case '3':
            std::cout << "Running with std::execution::par_unseq\n";
            RunTests(std::execution::par_unseq);
            break;
	
        default:
                std::cout << "Exiting program.\n";
                return 0;
        
    }
    

    return 0;
}

