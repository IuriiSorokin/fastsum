
#define BOOST_TEST_MODULE is_feature test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "generators.h"
#include "sum.h"
#include "dbg.h"
#include <iomanip>

using float_100  = boost::multiprecision::cpp_dec_float_100;


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
