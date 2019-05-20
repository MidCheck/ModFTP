/*************************************************************************
    > File Name: example.cpp
    > Author: MidCHeck
    > Mail: MidCheck@foxmail.com 
    > Created Time: Sat 18 May 2019 06:23:14 PM CST
 ************************************************************************/
#include <boost/regex.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string line;
    boost::regex pat( "^Subject: (Re: |Aw: )*(.*)" );

    while (std::cin)
    {
        std::getline(std::cin, line);
        boost::smatch matches;
        if (boost::regex_match(line, matches, pat))
            std::cout << matches[2] << std::endl;
    }
}
c++ -I /usr/local/include/boost/ example.cpp -o example -L/usr/local/lib/ -lboost_regex
