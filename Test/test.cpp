/*************************************************************************
    > File Name: test.cpp
    > Author: MidCHeck
    > Mail: MidCheck@foxmail.com 
    > Created Time: Sat 18 May 2019 05:48:44 PM CST
 ************************************************************************/
#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3) << " " );
}
