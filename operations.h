#include <cassert>
#include <map>
#include <cstdint>
#include <stack>
#include <string>

#ifdef ENABLE_ASSERTS
    #define gbiAssert(condition, message) assert((condition) && message)
#else
    #define gbiAssert(condition, message)
#endif

#define gbiStaticAssert(condition, message) static_assert((condition) && message)


namespace gbi
{
    typedef size_t SizeT;
    typedef uint8_t CharT;

    template<SizeT X, SizeT Y>
    struct Max
    {
        static const SizeT Value = X > Y ? X : Y;
    };

    struct Token
    {
        virtual bool isNode() const
        {
            return false;
        }

        virtual bool isSignal() const
        {
            return false;
        }

        virtual ~Token() {}
    };

    struct Signal : Token
    {
        enum SignalType
        {
            AdditionStart,
            FactorStart
        };

        bool isSignal() const override
        {
            return true;
        }

        SignalType m_Type;
    };

    struct Node : Token
    {
        enum NodeType
        {
            Number,
            Variable,
            Multiplication,
            Addition
        };

        enum ExtraQualifier
        {
            None,
            Inverse,  // "1/Variable" or "1/Number"
            Negative  // for negative Variables.
            // Note: Negative number should not be qualified as negative,
            // they are stored as negative float
        };

        NodeType m_Type;
        ExtraQualifier m_ExtraQualifiers;
    };

    // Most probably useless but… anywas :)
    gbiStaticAssert(sizeof(Signal) % sizeof(CharT) == 0, "Sorry, you seem to be using an unsupported type of architecture");

    typedef CharT TokenBucket[Max<sizeof(Signal), sizeof(Node)>::Value];

    class StateComputer
    {

    private:
        std::stack<TokenBucket> m_Buckets;
    };
}

#undef gbiStaticAssert
#undef gbiAssert
