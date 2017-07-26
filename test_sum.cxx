
#define BOOST_TEST_MODULE is_feature test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "generators.h"
#include "sum.h"
#include "dbg.h"
#include <iomanip>
#include <limits>

using float_100  = boost::multiprecision::cpp_dec_float_100;

BOOST_AUTO_TEST_CASE(test_exp)
{
    DBG( std::numeric_limits<double>::min_exponent );
    DBG( std::numeric_limits<double>::max_exponent );

    int e;
    DBG( std::numeric_limits<double>::min() );
    DBG( std::numeric_limits<double>::max() );
    DBG( get_exponent( std::numeric_limits<double>::min() ) );
           std::frexp( std::numeric_limits<double>::min(), &e ); DBG( e );
    DBG( get_exponent( std::numeric_limits<double>::max() ) );
           std::frexp( std::numeric_limits<double>::max(), &e ); DBG( e );
    DBG( get_exponent( - std::numeric_limits<double>::max() ) );
           std::frexp( - std::numeric_limits<double>::max(), &e ); DBG( e );

    DBG( get_exponent( -0.7e-308 ) );
           std::frexp( -0.7e-308, &e ); DBG( e );

    DBG( get_exponent( -0.7e-307 ) );
           std::frexp( -0.7e-307, &e ); DBG( e );

    DBG( get_exponent(  0.7e-307 ) );
           std::frexp(  0.7e-307, &e ); DBG( e );

    DBG( get_exponent( -0.7e307 ) );
           std::frexp( -0.7e307, &e ); DBG( e );

    DBG( get_exponent(  0.7e307 ) );
           std::frexp(  0.7e307, &e ); DBG( e );

    DBG( get_exponent(  0.7e308 ) );
           std::frexp(  0.7e308, &e ); DBG( e );

}

BOOST_AUTO_TEST_CASE(test_sum1)
{
    union U {
        double dval;

        struct {
            uint64_t mantisa  : 52;
            uint64_t exponent : 11;
            uint64_t sign     :  1;
        } parts;

    };

    std::cout << std::setprecision(18);

    auto uniform = [&](){ return std::uniform_real_distribution<double>(0.5, 1)( gRandomEngine() ); };

    // double a = 0.5;
    double a = uniform();
    double b = uniform();
    double c = a + b;

    auto& a_U = reinterpret_cast<U&>(a);
    auto& b_U = reinterpret_cast<U&>(b);
    auto& c_U = reinterpret_cast<U&>(c);

    DBG(a);
    DBG(b);
    DBG(c);

    std::cout << std::endl;

    DBG(a_U.dval);
    DBG(b_U.dval);

    std::cout << std::endl;

    DBG(a_U.parts.sign);
    DBG(a_U.parts.mantisa);
    DBG(a_U.parts.exponent);

    std::cout << std::endl;

    DBG(b_U.parts.sign);
    DBG(b_U.parts.mantisa);
    DBG(b_U.parts.exponent);

    std::cout << std::endl;

    DBG(c_U.parts.sign);
    DBG(c_U.parts.mantisa);
    DBG(c_U.parts.exponent);

    U d;

    d.parts.sign = a_U.parts.sign;
    d.parts.exponent = a_U.parts.exponent + 1;
    d.parts.mantisa  = (a_U.parts.mantisa + b_U.parts.mantisa)*2;

    std::cout << std::endl;

    DBG(d.dval);
    DBG(d.parts.sign);
    DBG(d.parts.mantisa);
    DBG(d.parts.exponent);



}
