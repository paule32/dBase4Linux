// gcc  -c -O2 --std=c++1y -D__BYTE_ORDER=__LITTLE_ENDIAN
//		-I/usr/include -I/usr/include/boost parser1.cc 

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include <iostream>
#include <string>

#include <cstring>
#include <cstdlib>

using namespace std;
using namespace boost::spirit;
using namespace boost::spirit::qi;

namespace client {
template <typename Iterator, typename Skipper>
struct my_test : public qi::grammar<Iterator, Skipper>
{
	my_test() : my_test::base_type(vs, "test")
	{
		vs =
		(
				lit("[")
			>>	*(char_ - char_("]"))
			>>  *(space)
			>>	*(char_ - char_("]"))
			>	lit("]")
		)
		;
	}
	rule<Iterator,Skipper> vs;
};
}

int main()
{
	typedef std::string::iterator iterator_t;

	try {
		std::string source = "[ ssssss ssss aaa ] ";
		typedef client::my_test <iterator_t, decltype(qi::space)> grammar;

		grammar pg;

		iterator_t iter = source.begin();
		iterator_t end  = source.end();

		bool ok = phrase_parse(iter,end, pg, space);
		if (ok) {
			std::cout
			<< "successful"
			<< std::endl;
			return 0;
		}
		std::cout << "error" << std::endl;
	}
	catch(expectation_failure<iterator_t>& e)
	{
		std::cout << "expected: " << e.what_ << std::endl;
	}

	return 0;
}

