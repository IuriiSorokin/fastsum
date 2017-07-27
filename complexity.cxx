
#define BOOST_TEST_MODULE is_feature test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "generators.h"
#include "benchmark.h"
#include "sum.h"
#include "dbg.h"

// auto generator = generate_uniform_positive;
auto generator = generate_exponential_positive;
// auto generator = generate_exponential_pos_neg_pairs;
// auto generator = generate_uniform_power_pos_neg_pairs;


BOOST_AUTO_TEST_CASE(test_sum1)
{
    std::cout << "=================== Complexity of sum1 =======================" << std::endl;
    evaluate_complexity(
            sum1,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5 },
            [](size_t n){ return n * n * std::log(n); },
            "n^2*log(n)"
    );
}



BOOST_AUTO_TEST_CASE(test_sum2)
{
    std::cout << "=================== Complexity of sum2 =======================" << std::endl;
    evaluate_complexity(
            sum2,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5 },
            [](size_t n){ return n * n; },
            "n^2"
    );
}



BOOST_AUTO_TEST_CASE(test_sum3)
{
    std::cout << "=================== Complexity of sum3 =======================" << std::endl;
    evaluate_complexity(
            sum3,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n * std::log(n); },
            "n*log(n)"
    );
}



BOOST_AUTO_TEST_CASE(test_sum3_construct_only)
{
    struct sum3_construct_only {
        static double
        sum(std::vector<double>& v)
        {
            std::multiset<double> set(v.begin(), v.end());
            return *set.begin();
        }
    };

    std::cout << "=================== Complexity of sum3_construct_only ========" << std::endl;
    evaluate_complexity(
            sum3_construct_only::sum,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n * std::log(n); },
            "n*log(n)"
    );
}



BOOST_AUTO_TEST_CASE(test_sum3_sum_only)
{
    struct sum3_sum_only {
        static std::multiset<double>& get_static_set() {
            static std::multiset<double> set;
            return set;
        }

        static double
        sum(std::vector<double>& v)
        {
            auto& set = get_static_set();
            while (set.size() > 1) {
                std::multiset<double>::const_iterator itA = set.begin();
                std::multiset<double>::const_iterator itB = ++set.begin();
                double c = *itA + *itB;
                set.erase(itA, ++itB);
                set.insert(c);
            }
            return !set.empty() ? *set.begin()
                                : 0.0;
        }

        static Series
        generate_and_construct_set(  size_t n ) {
            auto series = generator( n );
            get_static_set().clear();
            get_static_set().insert( series.v.begin(), series.v.end() );
            return series;
        }
    };

    std::cout << "=================== Complexity of sum3_sum_only ==============" << std::endl;
    evaluate_complexity(
            sum3_sum_only::sum,
            sum3_sum_only::generate_and_construct_set,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n * std::log(n); },
            "n*log(n)"
    );
}



BOOST_AUTO_TEST_CASE(test_sum_kahan_classic)
{
    std::cout << "=================== Complexity of kahan_classic ==============" << std::endl;
    evaluate_complexity(
            sum_kahan_classic,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n; },
            "n"
    );
}



BOOST_AUTO_TEST_CASE(test_sum_kahan_optimized)
{
    std::cout << "=================== Complexity of kahan_optimized ============" << std::endl;
    evaluate_complexity(
            sum_kahan_optimized,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n; },
            "n"
    );
}



BOOST_AUTO_TEST_CASE(test_sum_simple_double)
{
    std::cout << "=================== Complexity of sum_simple_double ==========" << std::endl;
    evaluate_complexity(
            sum_simple<double>,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n; },
            "n"
    );
}



BOOST_AUTO_TEST_CASE(test_sum_simple_long_double)
{
    std::cout << "=================== Complexity of sum_simple<long double> =====" << std::endl;
    evaluate_complexity(
            sum_simple<long double>,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n; },
            "n"
    );
}



BOOST_AUTO_TEST_CASE(test_sum_accurate)
{
    std::cout << "=============== Complexity of sum_fast_accurate ========" << std::endl;
    evaluate_complexity(
            sum_accurate,
            generator,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n; },
            "n"
    );
}



BOOST_AUTO_TEST_CASE(test_sum_accurate_power)
{
    std::cout << "=============== Complexity of sum_accurate_power ========" << std::endl;
    evaluate_complexity(
            sum_accurate,
            generate_uniform_power_pos_neg_pairs,
            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
            [](size_t n){ return n; },
            "n"
    );
}



//BOOST_AUTO_TEST_CASE(test_sum_f50)
//{
//    std::cout << "=============== Complexity of sum_fast_accurate ========" << std::endl;
//    evaluate_complexity(
//            sum_simple<float_50>,
//            // generator,
//            generate_exponential_pos_neg_pairs,
//            {1e2, 3.2e2, 1e3, 3.2e3, 1e4, 3.2e4, 1e5, 3.2e5, 1e6, 3.2e6, 1e7 },
//            [](size_t n){ return n; },
//            "n"
//    );
//}




