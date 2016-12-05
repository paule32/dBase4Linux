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
		vs.name("vs");
		symbol_endif.name("ENDIF");

		expression =
		int_ > lit("==") > int_
		;

		vs =
		(
			(symbol_if > expression)
			>>	*(
				(symsbols)
				)
			>>	*(
				(symbol_else)
				>>	*(
					(symsbols)
					)
				)
			>	symbol_endif
		)
		;

		symsbols =
		(
			int_ | vs
		)
		;

        symbol_if    = lexeme[no_case["if"]];
        symbol_else  = lexeme[no_case["else"]];
        symbol_endif = lexeme[no_case["endif"]];
	}

	rule<Iterator,Skipper>
		vs,
		symbol_if,
		symbol_else,
		symbol_endif, symsbols, expression
		;
};


#ifdef _ASDASASDASDASD__
template <typename Iterator, typename Skipper>
struct my_test : public qi::grammar<Iterator, Skipper>
{
	my_test() : my_test::base_type(vs, "test")
	{
		vs.name("vs");
		vs =
		lit("if") > ('(' > int_ > ')')
		>> *(
		    (*(char_("a-zA-Z0-9")) - "if" - "endif" > '=' > int_ ) |
			(vs)
			)
		>> *(
			lit("else")
			>> *(
			    (*(char_("a-zA-Z0-9")) - "if" - "endif" > '=' > int_ ) |
				(vs)
				)
			)
		>	lit("endif")
		;
	}

	rule<Iterator,Skipper> vs;
};
#endif

}

int main()
{
	typedef std::string::iterator iterator_t;

	try {
		std::string source = "if 1212 == 1212 else if 1212 == 1221 endif";
		typedef client::my_test      <iterator_t, decltype(qi::space)> grammar;

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
		std::cout << "eeerrror" << std::endl;
	}
	catch(expectation_failure<iterator_t>& e)
	{
		std::cout << "expected: " << e.what_ << std::endl;
	}

	return 0;
}

