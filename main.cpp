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

    struct operand :
        sor<bracketed_expression, identifier, number>
    {
    };

    struct factor :
        seq<
            operand,
            star<
                seq<
                    op<'*', '/'>,
                    operand
                >
            >
        >
    {
    };

    struct addition :
        seq<
            factor,
            star<
                seq<
                    op<'+', '-'>,
                    factor
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
    struct action<number>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "number: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<factor>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "factor: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<addition>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "addition: " << in.string() << std::endl;
        }
    };

    template<>
    struct action<bracketed_expression>
    {
        template< typename Input >
        static void apply( const Input& in)
        {
            std::cout << "bracketed_expression: " << in.string() << std::endl;
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

    std::string content("a = (1.5 * ((qwer * 12 + 45.12 * 7 * weoiru) * abc)) + 5 * (1 * 2 + 3 * 4) * (5 * 6 + 7 * 8)");
    std::vector<float> v;

    tao::pegtl::string_input<> in(content, "");
    bool result = tao::pegtl::parse< gbi::grammar, gbi::action >( in );

    return (result ? 0 : 1);
}
