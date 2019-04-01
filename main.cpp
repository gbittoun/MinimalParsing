#include <iostream>
#include <pegtl.hpp>
#include "operations.h"
#include <sstream>

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
        seq<identifier>
    {
    };

    struct plus_identifier :
        seq<identifier>
    {
    };

    struct signed_identifier :
        sor<
            seq<op<'-'>, minus_identifier>,
            plus_identifier
        >
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
    struct action<plus_identifier>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.pushVariable(in.string(), false);
        }
    };

    template<>
    struct action<minus_identifier>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.pushVariable(in.string(), true);
        }
    };

    template<>
    struct action<number>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            double value;
            std::stringstream ss;
            ss << in.string();
            ss >> value;

            states.pushNumber(value, false);
        }
    };

    template<>
    struct action<factor_start>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.pushSignal(Signal::SignalType::FactorStart);
        }
    };

    template<>
    struct action<div_term>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.setInverse();
        }
    };

    template<>
    struct action<addition_start>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.pushSignal(Signal::SignalType::AdditionStart);
        }
    };

    template<>
    struct action<minus_term>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.setNegative();
        }
    };

    template<>
    struct action<factor>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.close<Factor>();
        }
    };

    template<>
    struct action<addition>
    {
        template< typename Input >
        static void apply( const Input& in, StateComputer& states)
        {
            states.close<Addition>();
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
    gbi::StateComputer states;

    tao::pegtl::string_input<> in(content, "");
    bool result = tao::pegtl::parse< gbi::grammar, gbi::action >( in, states );

    return (result ? 0 : 1);
}
