/*
 * generators.h
 *
 *  Created on: 25 Jul 2017
 *      Author: sorokin
 */

#ifndef GENERATORS_H_
#define GENERATORS_H_

#include <random>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "dbg.h"

using float_50  = boost::multiprecision::cpp_dec_float_50;


template< typename RandomEngineType = std::mt19937_64 >
inline RandomEngineType&
gRandomEngine() {
    static auto e = RandomEngineType();
    return e;
}



struct Series
{
    std::vector<double> v;
    double sum = 0;
};



inline Series
generate_uniform_positive( size_t n )
{
    auto distr = std::uniform_real_distribution<double>(0, 1);
    Series series;
    float_50 sum = 0;
    series.v.reserve( n );
    while( series.v.size() < n ) {
        double x = distr(gRandomEngine());
        series.v.push_back( x );
        sum += x;
    }
    std::shuffle( series.v.begin(), series.v.end(), gRandomEngine() );
    series.sum = static_cast<double>(sum);
    return series;
}



inline Series
generate_uniform_pos_neg_pairs( size_t n )
{
    auto distr = std::uniform_real_distribution<double>(0, 1);
    Series series;
    series.v.reserve( n );

    while( series.v.size() < n-2 ) {
        double x = distr(gRandomEngine());
        series.v.push_back( x );
        series.v.push_back( -x );
    }

    double x = distr(gRandomEngine());
    series.v.push_back( x );
    series.sum = x;

    if( series.v.size() < n ) {
        series.v.push_back( 0 );
    }

    std::shuffle( series.v.begin(), series.v.end(), gRandomEngine() );

    return series;
}



inline Series
generate_exponential_pos_neg_pairs( size_t n )
{
    auto distr = std::exponential_distribution<double>(0.01);
    Series series;
    series.v.reserve( n );

    while( series.v.size() < n-2 ) {
        double x = distr(gRandomEngine());
        series.v.push_back( x );
        series.v.push_back( -x );
    }

    double x = distr(gRandomEngine());
    series.v.push_back( x );
    series.sum = x;

    if( series.v.size() < n ) {
        series.v.push_back( 0 );
    }

    std::shuffle( series.v.begin(), series.v.end(), gRandomEngine() );

    return series;
}



inline Series
generate_exponential_positive( size_t n )
{
    auto distr = std::exponential_distribution<double>(0.01);
    Series series;
    series.v.reserve( n );
    float_50 sum = 0;

    while( series.v.size() < n ) {
        double x = distr(gRandomEngine());
        series.v.push_back( x );
        sum += x;
    }

    series.sum = static_cast<double>( sum );

    std::shuffle( series.v.begin(), series.v.end(), gRandomEngine() );

    return series;
}



inline Series
generate_uniform_power_pos_neg_pairs( size_t n )
{
    auto mantisa = std::uniform_real_distribution<double>(0,1);
    auto power = std::uniform_int_distribution<int>(-250,250);
    Series series;
    series.v.reserve( n );

    while( series.v.size() < n-2 ) {
        auto m = mantisa( gRandomEngine() );
        auto p = power( gRandomEngine() );
        double x = m * std::pow( 10, p );
        series.v.push_back( x );
        series.v.push_back( -x );
    }

    auto m = mantisa( gRandomEngine() );
    auto p = power( gRandomEngine() );
    double x = m * std::pow( 10, p );

    series.v.push_back( x );
    series.sum = x;

    if( series.v.size() < n ) {
        series.v.push_back( 0 );
    }

    std::shuffle( series.v.begin(), series.v.end(), gRandomEngine() );

    return series;
}




#endif /* GENERATORS_H_ */
