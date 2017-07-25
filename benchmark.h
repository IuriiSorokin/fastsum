/*
 * benchmark.h
 *
 *  Created on: 25 Jul 2017
 *      Author: sorokin
 */

#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include <chrono>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iostream>
#include <boost/format.hpp>


struct BenchmarkResult
{
    size_t n_items = 0;
    int    n_runs    = 0;
    int    n_discard = 0;
    double time_us_min = 0;
    double time_us_avg = 0;
    double time_us_best_avg = 0;
    double time_us_max = 0;
    double rel_err_min = 0;
    double rel_err_avg = 0;
    double rel_err_max = 0;
};



template<typename AlgorithmT, typename GeneratorT>
BenchmarkResult
benchmark( AlgorithmT sum_algo, GeneratorT generate, size_t nItems, int nRuns, int nDiscard) {

    std::vector<double>  times;
    std::vector<double>  rel_erorrs;

    for( int iRun = 0; iRun < nRuns; ++iRun ) {
        auto series = generate( nItems );

        const auto start = std::chrono::high_resolution_clock::now();
        const double sum = sum_algo( series.v );
        const auto stop  = std::chrono::high_resolution_clock::now();

        const double time        = std::chrono::duration_cast<std::chrono::microseconds>( stop - start ).count();
        const double rel_error    = std::abs( sum - series.sum ) / series.sum;

        times.push_back( time );
        rel_erorrs.push_back( rel_error );
    }

    BenchmarkResult result;
    result.n_items   = nItems;
    result.n_runs    = nRuns;
    result.n_discard = nDiscard;
    result.time_us_min = *std::min_element(times.begin(), times.end());
    result.time_us_avg =  std::accumulate( times.begin(), times.end(), 0.) / times.size();
    result.time_us_max = *std::max_element(times.begin(), times.end());
    result.rel_err_min = *std::min_element(rel_erorrs.begin(), rel_erorrs.end());
    result.rel_err_avg =  std::accumulate( rel_erorrs.begin(), rel_erorrs.end(), 0.) / rel_erorrs.size();
    result.rel_err_max = *std::max_element(rel_erorrs.begin(), rel_erorrs.end());
    std::sort( times.begin(), times.end() );
    times.erase( times.end() - nDiscard, times.end() );
    result.time_us_best_avg = std::accumulate( times.begin(), times.end(), 0.) / times.size();
    return result;
}



template<typename AlgorithmT, typename GeneratorT, typename ExpectedComplexityT>
void
evaluate_complexity( AlgorithmT sum_algo, GeneratorT generate, std::vector<double> nItemsVect, ExpectedComplexityT expected_complexity, std::string expected_complexity_string, int nRuns = 7, int nDiscard = 2 )
{
    std::cout << "            Avg time of      Avg relative " << std::endl;
    std::cout << " N Items   best " << nRuns - nDiscard << "/" << nRuns << " runs   error all runs   time/(" << expected_complexity_string << ")" << std::endl;
    for( auto nItems : nItemsVect ) {
        auto result = benchmark( sum_algo, generate, nItems, nRuns, nDiscard );
        std::cout << boost::format("%8e    %8e    %8e    %8e")
                % result.n_items
                % result.time_us_best_avg
                % result.rel_err_avg
                % (result.time_us_best_avg / ( expected_complexity(nItems) ) ) << std::endl;
    }
}

#endif /* BENCHMARK_H_ */
