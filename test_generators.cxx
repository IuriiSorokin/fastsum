
#define BOOST_TEST_MODULE is_feature test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "generators.h"
#include "sum.h"
#include "dbg.h"

using float_100  = boost::multiprecision::cpp_dec_float_100;
using float_500  = boost::multiprecision::number< boost::multiprecision::cpp_dec_float<500> >;

BOOST_AUTO_TEST_CASE(test_exponential_positive)
{
    auto s = generate_exponential_positive( 10000000 );
    BOOST_CHECK_EQUAL( s.sum, sum_simple< float_100 >( s.v ) );
}


BOOST_AUTO_TEST_CASE(test_uniform_power_pos_neg_pairs)
{
    auto s = generate_uniform_power_pos_neg_pairs( 1000000 );
    BOOST_CHECK_EQUAL( s.sum, sum_simple<float_500>( s.v ) );
}



BOOST_AUTO_TEST_CASE(test_uniform_power_pos_neg_pairs_print)
{
    //    auto s = generate_uniform_power_pos_neg_pairs( 1000000 );
    //    for( auto x : s.v ) {
    //        DBG( x );
    //    }

    // BOOST_CHECK_EQUAL( s.sum, sum_simple<float_500>( s.v ) );
}
