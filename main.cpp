#include <iostream>
#include <pegtl.hpp>
// #include <pegtl/analyze.hpp>

namespace gbi
{
    using namespace tao;

    // Operator surrounded by spaces
    template<char... operator_char>
    struct op :
        pegtl::seq<
            pegtl::star<pegtl::space>,
            pegtl::one<operator_char...>,
            pegtl::star<pegtl::space>
        >
    {
    };
}

namespace gbi
{
    using namespace tao::pegtl;

    struct number :
        seq<
            opt<op< '+', '-' >>,
            plus< digit >,
            opt<seq<one<'.'>, plus<digit>>>
        >
    {
    };

    struct bracketed_expression;

    struct minus_identifier :
        seq<op<'-'>>
    {
    };

    struct signed_identifier :
        seq<opt<minus_identifier>, identifier>
    {
    };

    struct operand :
        sor<bracketed_expression, signed_identifier, number>
    {
    };

    struct factor_start :
        seq<operand>
    {
    };

    struct mult_term :
        seq<operand>
    {
    };

    struct div_term :
        seq<operand>
    {
    };

    struct factor :
        seq<
            factor_start,
            star<
                sor<
                    seq<op<'*'>, mult_term>,
                    seq<op<'/'>, div_term>
                >
            >
        >
    {
    };

    struct addition_start :
        seq<factor>
    {
    };

    struct plus_term :
        seq<factor>
    {
    };

    struct minus_term :
        seq<factor>
    {
    };

    struct addition :
        seq<
            addition_start,
            star<
                sor<
                    seq<op<'+'>, plus_term>,
                    seq<op<'-'>, minus_term>
                >
            >
        >
    {
    };

    struct expression :
        seq<addition>
    {
    };

    struct bracketed_expression :
        seq<op<'('>, expression, op<')'>>
    {
    };

    struct assignment :
        seq<identifier, op<'='>, expression>
    {
    };

    struct grammar :
        must<sor<assignment, expression>, eolf>
    {
    };

    template<class Rule>
    struct action :
        nothing<Rule>
    {
    };

    template<>
    struct action<identifier>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "identifier: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<minus_identifier>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "minus_identifier: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<number>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "number: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<factor_start>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "factor_start: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<mult_term>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "mult_term: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<div_term>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "div_term: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<addition_start>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "addition_start: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<plus_term>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "plus_term: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<minus_term>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "minus_term: " << in.string() << std::endl;
        }
    };
}

#include <pegtl/analyze.hpp>

int main()
{
    if( tao::pegtl::analyze< gbi::grammar >() != 0 ) {
        std::cout << "there are problems" << std::endl;
        return 1;
    }

    std::string content("(1.5 * ((qwer * 12 + 45.12 * 7 * -   weoiru) * abc)) + 5 / (1 * 2 + 3 * 4) * (5 * 6 + 7 * 8)");
    std::vector<float> v;

    tao::pegtl::string_input<> in(content, "");
    bool result = tao::pegtl::parse< gbi::grammar, gbi::action >( in );

    return (result ? 0 : 1);
}
