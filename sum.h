
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
sum_fast( const std::vector<double>& v )
{
    std::array<double, 2048 > part_sums;
    part_sums.fill( 0 );

    for( auto x : v ) {
        part_sums.at( get_exponent(x) ) += x;
    }

    double sum = 0;
    for( auto x : part_sums ) {
        sum += x;
    }

    return sum;
}



inline double
sum_fast_optimized( std::vector<double>& v )
{
    std::array<double, 2048 > part_sums0;
    std::array<double, 2048 > part_sums1;
    part_sums0.fill( 0 );
    part_sums1.fill( 0 );

    double* x = v.data();
    const double* x_e = v.data() + 2*(v.size()/2);

    while( x < x_e ) {
        part_sums0[ get_exponent(*x)     ] += *x;
        part_sums1[ get_exponent(*(x+1)) ] += *(x+1);
        x += 2;
    }

    if( v.size() % 2 ) {
        part_sums0[ get_exponent( v.back() ) ] += v.back();
    }

    double sum0 = 0;
    double sum1 = 0;
    for( auto x : part_sums0 ) {
        sum0 += x;
    }
    for( auto x : part_sums1 ) {
        sum1 += x;
    }

    return sum0 + sum1;
}



inline double
sum_fast_accurate( std::vector<double>& v )
{
    std::cout << std::setprecision( 18 );
    std::array<double, 2048 > part_sums;
    part_sums.fill( 0 );

    size_t i = 0;

    while( i < v.size() ) {
//        std::cout << "\n\n";
//        DBG(i);

        double& x = v.at( i );
//        DBG(x);

        const size_t idx = get_exponent( x );
//        DBG(idx);

        double& old_sum = part_sums.at(idx);
//        DBG(old_sum);

        if( old_sum == 0.L ) {
            old_sum = x;
            ++i;
            continue;
        }

        const double new_sum = old_sum + x;
//        DBG(new_sum);

        const double increment = new_sum - old_sum;
//        DBG(increment);

        const double error = x - increment;
//        DBG(error);

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



//inline double
//sum_fast_accurate( std::vector<double>& v )
//{
//    std::cout << std::setprecision( 18 );
//    std::array<double, 2048 > part_sums;
//    part_sums.fill( 0 );
//
//    size_t i = 0;
//
//    while( i < v.size() ) {
////        std::cout << "\n\n";
////        DBG(i);
//
//        double& x = v.at( i );
////        DBG(x);
//
//        const size_t idx = get_exponent( x );
////        DBG(idx);
//
//        double& old_sum = part_sums.at(idx);
////        DBG(old_sum);
//
//        if( old_sum == 0.L ) {
//            old_sum = x;
//            ++i;
//            continue;
//        }
//
//        const double new_sum = old_sum + x;
////        DBG(new_sum);
//
//        const double increment = new_sum - old_sum;
////        DBG(increment);
//
//        const double error = x - increment;
////        DBG(error);
//
//        x = new_sum;
//        old_sum = 0;
//
//        if( error != 0.L ) {
//            --i;
//            v.at(i) = error;
//        }
//    }
//
//    for( size_t j = 0; j < 3; ++j ) {
//        for( size_t i = 1; i < part_sums.size(); ++i) {
//            auto& xi = part_sums.at(i);    // i-th
//            auto& xp = part_sums.at(i-1);  // previous
//            auto xi_new = xi + xp;
//            auto increment = xi_new - xi;
//            auto error = xp - increment;
//            xp = error;
//            xi = xi_new;
//        }
//    }
////     exit(0);
//
//    return part_sums.back();
//}



inline double
sum_fast_accurate2( std::vector<double>& v )
{
    std::cout << std::setprecision( 18 );
    std::array<double, 2048 > part_sums_pos;
    std::array<double, 2048 > part_sums_neg;
    part_sums_pos.fill( 0 );
    part_sums_neg.fill( 0 );

    size_t i = 0;

    while( i < v.size() ) {
//        std::cout << "\n\n";
//        DBG(i);

        double& x = v.at( i );
//        DBG(x);

        const size_t idx = get_exponent( x );
//        DBG(idx);

        if( x > 0 ) {
            double& part_sum_neg = part_sums_neg.at(idx);
            if( part_sum_neg != 0.L ) {
                x = x + part_sum_neg; // no numerical error, as numbers are of opposite sign and same exponent
                part_sum_neg = 0.L;
                continue;
            }

            double& part_sum_pos = part_sums_pos.at(idx);
            if( part_sum_pos == 0.L ) {
                part_sum_pos = x;
                ++i;
                continue;
            }

            const double sum = part_sum_pos + x; // numerical error error here
            const double increment = sum - part_sum_pos;
            const double error = x - increment;
            part_sum_pos = 0.L;
            x = sum;
            --i;
            v.at(i) = error;
        } else {
            double& part_sum_pos = part_sums_pos.at(idx);
            if( part_sum_pos != 0.L ) {
                x = x + part_sum_pos; // no numerical error, as numbers are of opposite sign and same exponent
                part_sum_pos = 0.L;
                continue;
            }

            double& part_sum_neg = part_sums_neg.at(idx);
            if( part_sum_neg == 0.L ) {
                part_sum_neg = x;
                ++i;
                continue;
            }

            const double sum = part_sum_neg + x; // numerical error error here
            const double increment = sum - part_sum_neg;
            const double error = x - increment;
            part_sum_neg = 0.L;
            x = sum;
            --i;
            v.at(i) = error;
        }
    }

    for( size_t i = 0; i < part_sums_pos.size(); ++i ) {
        part_sums_pos.at(i) += part_sums_neg.at(i);
    }

    for( size_t j = 0; j < 3; ++j ) {
        for( size_t i = 1; i < part_sums_pos.size(); ++i) {
            auto& xi = part_sums_pos.at(i);    // i-th
            auto& xp = part_sums_pos.at(i-1);  // previous
            auto xi_new = xi + xp;
            auto increment = xi_new - xi;
            auto error = xp - increment;
            xp = error;
            xi = xi_new;
        }
    }

    return part_sums_pos.back();
}




inline double
sum_fast_stable( std::vector<double>& v )
{
    std::array<double, 2048 > part_sums;
    part_sums.fill( 0 );

    size_t i = 0;
    while( i < v.size() ) {
        // DBG(i);
        auto x = v.at(i);
        // DBG(x);
        auto idx = get_exponent( x );
        auto& part_sum = part_sums.at(idx);

        auto part_sum_new = x + part_sum;
        auto actually_added = part_sum_new - part_sum;
        part_sum = part_sum_new;
        auto diff = x - actually_added;
        if( diff != 0 ) {
            v.at(i) = diff;
            --i;
        }
        if( get_exponent(part_sum_new) != idx ) {
            v.at(i) = part_sum_new;
            part_sum = 0;
            --i;
        }
        ++i;
    }

    double sum = 0;
    for( auto x : part_sums ) {
        sum += x;
    }

    return sum;
}



inline double
sum_fast3( const std::vector<double>& v )
{
    std::array<double, 2048 > part_sums1;
    std::array<double, 2048 > part_sums2;
    part_sums1.fill( 0 );
    part_sums2.fill( 0 );

    for( size_t i = 0; i < v.size(); i+= 2 ) {

        int exponent1 = 0;
        std::frexp(v.at(i), &exponent1 );
        int exponent2 = 0;
        std::frexp(v.at(i+1), &exponent2 );
        part_sums1.at( exponent1 + 1024 ) += v.at(i);
        part_sums2.at( exponent2 + 1024 ) += v.at(i+1);

    }


    double sum1 = 0;
    double sum2 = 0;

    const double* x1  = part_sums1.data();
    const double* x2  = part_sums2.data();
    const double* const e1  = x1 + part_sums1.size();
    // const double* e2  = x2 + part_sums2.size();

    while( x1 < e1 ) {
        sum1 += *x1;
        sum2 += *x2;
        ++x1;
        ++x2;
    }

    return sum1 + sum2;
}





//inline double
//sum_fast_safe( const std::vector<double>& v )
//{
//    constexpr size_t exp_swing  = std::numeric_limits<double>::max_exponent - std::numeric_limits<double>::min_exponent;
//    constexpr int    exp_offset = std::numeric_limits<double>::min_exponent;
//    std::array<double, exp_swing > part_sums; // non-static is faster
//    part_sums.fill( 0 );
//
//    int exponent = 0;
//    for( auto x : v ) {
//        std::frexp(x, &exponent );
//        part_sums.at( exponent - exp_offset ) += x;
//    }
//
//    double sum = 0;
//    for( auto x : part_sums ) {
//        sum += x;
//    }
//
//    return sum;
//}



//inline double
//sum_fast3s( const std::vector<double>& v )
//{
//    union U {
//       double dval;
//       struct {
//           uint64_t mantisa  : 52;
//           uint8_t  explow   :  3;
//           uint16_t exphigh  :  8;
//           uint16_t sign     :  1;
//       } comp;
//    };
//
//    std::array<double,  256 > part_sums0;
//    part_sums0.fill( 0 );
//    std::array<double,  256 > part_sums1;
//    part_sums1.fill( 0 );
//
//    const double* x  = v.data();
//    const double* const e  = x + v.size();
//
//    while( x < e ) {
//        part_sums0[ reinterpret_cast<const U*>(x  )->comp.exphigh ] += *x;
//        part_sums1[ reinterpret_cast<const U*>(x+1)->comp.exphigh ] += *(x+1);
//        x += 2;
//    }
//
//
//    double sum0 = 0;
//    double sum1 = 0;
//
//    const double* x0  = part_sums0.data();
//    const double* x1  = part_sums1.data();
//    const double* const e0  = x0 + part_sums0.size();
//    // const double* e2  = x2 + part_sums2.size();
//
//    while( x0 < e0 ) {
//        sum0 += *x0;
//        sum1 += *x1;
//        ++x0;
//        ++x1;
//    }
//
//    return sum0 + sum1;
//}


//
//inline double
//sum_fast4( const std::vector<double>& v )
//{
//    union U {
//        double dval;
//        struct {
//            uint64_t mantisa  : 52;
//            // uint64_t unused   :  1;
//            uint64_t exponent : 11;
//            uint64_t sign     :  1;
//        } parts;
//    };
//
//    std::array<double, 2048 > part_sums0;
//    std::array<double, 2048 > part_sums1;
//    std::array<double, 2048 > part_sums2;
//    std::array<double, 2048 > part_sums3;
//    part_sums0.fill( 0 );
//    part_sums1.fill( 0 );
//    part_sums2.fill( 0 );
//    part_sums3.fill( 0 );
//
//    const double* x  = v.data();
//    const double* const e  = x + v.size();
//
//    while( x < e ) {
//        part_sums0[ reinterpret_cast<const U*>(x  )->parts.exponent ] += *x;
//        part_sums1[ reinterpret_cast<const U*>(x+1)->parts.exponent ] += *(x+1);
//        part_sums2[ reinterpret_cast<const U*>(x+2)->parts.exponent ] += *(x+2);
//        part_sums3[ reinterpret_cast<const U*>(x+3)->parts.exponent ] += *(x+3);
//        x += 4;
//    }
//
//    double sum0 = 0;
//    double sum1 = 0;
//
//    const double* x0  = part_sums0.data();
//    const double* x1  = part_sums1.data();
//    const double* x2  = part_sums2.data();
//    const double* x3  = part_sums3.data();
//    const double* const e1  = x1 + part_sums1.size();
//
//    while( x1 < e1 ) {
//        sum0 += *x0;
//        sum0 += *x1;
//        sum1 += *x2;
//        sum1 += *x3;
//        ++x0;
//        ++x1;
//        ++x2;
//        ++x3;
//    }
//
//    return sum1 + sum0;
//}



#endif /* SUM_H_ */
