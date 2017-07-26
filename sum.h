
#ifndef SUM_H_
#define SUM_H_

#include <vector>
#include <algorithm>
#include <set>
#include <cmath>
#include <limits>
#include <boost/numeric/conversion/cast.hpp>

inline double
sum1(std::vector<double>& v)
{
    if (v.empty()) {
        return 0.0;
    }
    for(size_t i = 0; i < v.size() - 1; ++i) {
        std::sort(v.begin()+i, v.end());
        v[i+1] += v[i];
    }
    return v.back();
}



inline double
sum2(std::vector<double>& v)
{
    if (v.empty()) {
        return 0.0;
    }
    for(size_t i = 0; i < v.size() - 1; ++i) {
        std::partial_sort(v.begin() + i, v.begin() + i + 2, v.end());
        v[i+1] += v[i];
    }
    return v.back();
}



inline double
sum3(std::vector<double>& v)
{
    std::multiset<double> set(v.begin(), v.end());
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



template<typename T>
inline double
sum_simple( const std::vector<double>&v ) {
    T sum = 0;
    for( auto e : v ) {
        sum += e;
    }
    return static_cast<double>(sum);
}



inline double
sum_fast_safe( const std::vector<double>& v )
{
    constexpr size_t exp_swing  = std::numeric_limits<double>::max_exponent - std::numeric_limits<double>::min_exponent;
    constexpr int    exp_offset = std::numeric_limits<double>::min_exponent;
    std::array<double, exp_swing > part_sums; // non-static is faster
    part_sums.fill( 0 );

    int exponent = 0;
    for( auto x : v ) {
        std::frexp(x, &exponent );
        part_sums.at( exponent - exp_offset ) += x;
    }

    double sum = 0;
    for( auto x : part_sums ) {
        sum += x;
    }

    return sum;
}



inline double
sum_kahan_classic( std::vector<double>& v )
{
    double sum   = 0;
    double carry = 0;

    for( auto x : v ) {
        double y = x - carry;
        double sum_new = sum + y;
        double increment = sum_new - sum;
        carry = increment - y;
        sum = sum_new;
    }

    return sum + carry;
}



inline double
sum_kahan_optimized( std::vector<double>& v )
{
    double sum_high = 0;
    double sum_low  = 0;

    for( auto x : v ) {
        double sum_high_old = sum_high;
        sum_high += x;
        double increment = sum_high - sum_high_old;
        sum_low += x - increment;
    }

    return sum_high + sum_low;
}



inline size_t
get_exponent_decode_iec559( double x ) {
    static_assert( std::numeric_limits<double>::is_iec559 );

    union U {
        double value;
        struct {
            size_t mantisa  : 52;
            size_t exponent : 11;
            size_t sign     :  1;
        } parts;
    };

    static_assert( sizeof(U) == sizeof(double) );

    return reinterpret_cast<U&>( x ).parts.exponent;
}



inline size_t
get_mantisa( double x ) {
    static_assert( std::numeric_limits<double>::is_iec559 );

    union U {
        double value;
        struct {
            size_t mantisa  : 52;
            size_t exponent : 11;
            size_t sign     :  1;
        } parts;
    };

    static_assert( sizeof(U) == sizeof(double) );

    return reinterpret_cast<U&>( x ).parts.mantisa;
}




inline size_t
get_exponent_frexp( double x ) {
    int exponent;
    frexp( x, &exponent );
    return boost::numeric_cast<size_t>(exponent - std::numeric_limits<double>::min_exponent + 1);
}



inline size_t
get_exponent( double x ) {
    if( std::numeric_limits<double>::is_iec559 ) {
        return get_exponent_decode_iec559( x );
    }
    return get_exponent_frexp( x );
}



inline double
sum_accurate( std::vector<double>& v )
{
    std::cout << std::setprecision( 18 );
    std::array<double, 2048 > part_sums;
    part_sums.fill( 0 );

    size_t i = 0;

    while( i < v.size() ) {
        double& x = v.at( i );
        const size_t idx = get_exponent( x );
        double& old_sum = part_sums.at(idx);
        if( old_sum == 0.L ) {
            old_sum = x;
            ++i;
            continue;
        }

        const double new_sum = old_sum + x;
        const double increment = new_sum - old_sum;
        const double error = x - increment;

        x = new_sum;
        old_sum = 0;

        if( error != 0.L ) {
            --i;
            v.at(i) = error;
        }
    }

    auto sum_and_leave_errors = [&]() {
        double sum = 0;
        for( size_t i = part_sums.size(); i-- > 0; ) {
            double& x = part_sums.at(i);
            double sum_new = sum + x;
            double increment = sum_new - sum;
            double error = x - increment;
            sum = sum_new;
            part_sums.at(i) = error;

            if( get_exponent(sum) > i + 60 ) {
                break;
            }
        }
        return sum;
    };

    double sum_high = sum_and_leave_errors();
    double sum_low  = sum_and_leave_errors();

    return sum_high + sum_low ;
}

#endif /* SUM_H_ */
