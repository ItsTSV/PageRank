#include <algorithm>
#include <iostream>
#include <vector>
#include <thread>
#include <sstream>
#include <chrono>
#include <fstream>
#include <map>
#include <numeric>
#include <omp.h>

/*
 *  Warning: C++ code done by someone who only used Python, JavaScript and Ruby last two years
 *  Lots of random, ugly and suboptimal stuff *will* occur
 */

// Test if OpenMP works
#define USE_OMP
void testOMP() {
#ifdef USE_OMP
    std::cout << "OpenMP works, hello from thread " << omp_get_thread_num() << std::endl;
#endif
}

// Read the chart parallely
void load_chart_parallel(std::map<int, std::vector<int> > &chart) {
    // Prepare file, tmp vector and tmp line
    std::ifstream file("web-BerkStan.txt");
    std::vector<std::string> lines;
    std::string line;

    // Get node count from the first line, prepare the empty chart
    std::getline(file, line);
    int node_count = std::stoi(line);
    for (int i = 1; i <= node_count; i++) {
        chart[i] = std::vector<int>();
    }

    // Read the whole file
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    // Use OMP parallel to load the lines and critical section to merge the vectors
#pragma omp parallel
    {
        // Used to store *part* of the file
        std::map<int, std::vector<int> > local_chart;

        // Parallely load the lines into local charts
#pragma omp for
        for (auto i = 0; i < lines.size(); i++) {
            int node, edge;
            std::istringstream iss(lines[i]);

            if (iss >> node >> edge) {
                local_chart[node].push_back(edge);
            }
        }

        // Use critical section to make sure no race condition mess occurs
#pragma omp critical
        {
            // Merge local charts into the one passed via reference
            for (const auto &[node, edges]: local_chart) {
                chart[node].insert(
                    chart[node].end(),
                    edges.begin(),
                    edges.end()
                );
            }
        }
    }
} // Python would never :(

// Run the PageRank algorithm, parallelize via OpenMP
std::vector<double> run_page_rank(std::map<int, std::vector<int>> &chart, double d=0.85, int iterations=50, int threshold=0.0001) {
    // Prepare size of the chart and PageRank values
    int N = chart.size();
    std::vector<double> PR(N, 1.0 / N);

    // Run the PageRank algorithm
    for (int i = 0; i < iterations; i++) {
        std::vector<double> new_PR(N, 0.0);

        // Calculate the new PageRank values
#pragma omp parallel for
        for (int j = 0; j < N; j++) {
            for (const auto &edge: chart[j]) {
                new_PR[edge] += d * PR[j] / chart[j].size();
            }
        }

        // Update the PageRank values, check if the values converged
        bool converged = true;
        for (int j = 0; j < N; j++) {
            PR[j] = new_PR[j] + (1 - d) / N;
            if (std::abs(new_PR[j] - PR[j]) > threshold) {
                converged = false;
            }
        }

        // If yes, break (this would take few hours in Python)
        if (converged) {
            break;
        }
    }

    return PR;
}

int main() {
    // Set OMP stuff
    omp_set_num_threads(8);
    testOMP();

    // Prepare chart
    std::map<int, std::vector<int> > graph;

    // Load chart
    auto start = std::chrono::high_resolution_clock::now();
    load_chart_parallel(graph);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Loading took " << time.count() << " ms\n";
    std::cout << "Loaded " << graph.size() << " nodes\n";

    // Calculate the PageRank algorithm
    start = std::chrono::high_resolution_clock::now();
    auto final_pr = run_page_rank(graph);
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Sort the PageRank values
    std::vector<size_t> indices(final_pr.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::partial_sort(indices.begin(), indices.begin() + 5, indices.end(),
                      [&](size_t A, size_t B) {
                         return final_pr[A] > final_pr[B];
                      });

    // Print results
    std::cout << "PageRank took " << time.count() << " ms\n";
    std::cout << "PageRank best values:\n";
    int count = 0;
    for (const auto &index: indices) {
        std::cout << "Node " << index << " -> " << final_pr[index] << "\n";
        if (++count == 5) {
            break;
        }
    }
    return 0;
}
