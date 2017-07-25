
#define BOOST_TEST_MODULE is_feature test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "generators.h"
#include "sum.h"
#include "dbg.h"

using float_100  = boost::multiprecision::cpp_dec_float_100;

//BOOST_AUTO_TEST_CASE(test_sum1)
//{
//    auto s = generate_exponential_positive( 10000000 );
//    BOOST_CHECK_EQUAL( s.sum, sum_simple< float_100 >( s.v ) );
//}
//

BOOST_AUTO_TEST_CASE(test_sum1)
{
    auto s = generate_exponential_positive( 10000000 );
    BOOST_CHECK_EQUAL( s.sum, sum_simple< float_100 >( s.v ) );
}
