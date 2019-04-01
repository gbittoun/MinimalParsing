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


namespace gbi
{
    typedef size_t SizeT;
    typedef uint8_t CharT;

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

        Signal(SignalType type) :
            m_Type(type)
        {
        }

        SignalType m_Type;
    };

    struct Node : Token
    {
        enum class ExtraQualifier : CharT
        {
            None = 0x0,
            Negative = 0x1,
            Inverse = 0x2
        };

        Node():
            m_Qualifiers(ExtraQualifier::None)
        {
        };

        Node(ExtraQualifier qualifiers):
            m_Qualifiers(qualifiers)
        {
        };

        bool isNode() const override
        {
            return true;
        }

        void setInverse()
        {
            m_Qualifiers = static_cast<ExtraQualifier>(static_cast<CharT>(m_Qualifiers) | static_cast<CharT>(ExtraQualifier::Inverse));
        }

        void setNegative()
        {
            m_Qualifiers = static_cast<ExtraQualifier>(static_cast<CharT>(m_Qualifiers) | static_cast<CharT>(ExtraQualifier::Negative));
        }

        ExtraQualifier m_Qualifiers;
    };

    Node::ExtraQualifier operator&(const Node::ExtraQualifier& a, const Node::ExtraQualifier& b)
    {
        return static_cast<Node::ExtraQualifier>(static_cast<CharT>(a) & static_cast<CharT>(b));
    }

    Node::ExtraQualifier operator|(const Node::ExtraQualifier& a, const Node::ExtraQualifier& b)
    {
        return static_cast<Node::ExtraQualifier>(static_cast<CharT>(a) | static_cast<CharT>(b));
    }

    struct OperatorNode : Node
    {
        void push(std::shared_ptr<Node> node)
        {
            m_Terms.push_back(node);
        }

        std::vector<std::shared_ptr<Node>> m_Terms;
    };

    struct Addition : OperatorNode
    {
        static const Signal::SignalType associatedStartSignal = Signal::AdditionStart;
    };

    struct Factor : OperatorNode
    {
        static const Signal::SignalType associatedStartSignal = Signal::FactorStart;
    };

    struct Variable : Node
    {
        Variable(const std::string& variableName, ExtraQualifier qualifiers):
            Node(qualifiers),
            m_VariableName(variableName)
        {
        }

        std::string m_VariableName;
    };

    struct Number : Node
    {
        Number(double value, ExtraQualifier qualifiers):
            Node(qualifiers),
            m_Value(value)
        {
        }

        double m_Value;
    };

    class StateComputer
    {
    public:
        void pushSignal(Signal::SignalType type)
        {
            m_Tokens.push(std::make_shared<Signal>(type));
        }

        void pushVariable(const std::string& variableName, bool negative)
        {
            std::shared_ptr<Variable> variable = std::make_shared<Variable>(
                variableName, negative ? Node::ExtraQualifier::Negative : Node::ExtraQualifier::None);
            m_Tokens.push(variable);
        }

        void pushNumber(double value, bool negative)
        {
            m_Tokens.push(std::make_shared<Number>(
                value, negative ? Node::ExtraQualifier::Negative : Node::ExtraQualifier::None));
        }

        void setInverse()
        {
            gbiAssert(m_Tokens.top()->isNode(), "Only nodes can be set as division terms");

            std::static_pointer_cast<Node>(m_Tokens.top())->setInverse();
        }

        void setNegative()
        {
            gbiAssert(m_Tokens.top()->isNode(), "Only nodes can be set as minus terms");

            std::static_pointer_cast<Node>(m_Tokens.top())->setNegative();
        }

        template<typename T>
        void close()
        {
            std::shared_ptr<T> operatorNode = std::make_shared<T>();
            std::shared_ptr<Token> token = m_Tokens.top();
            m_Tokens.pop();

            while(!token->isSignal())
            {
                gbiAssert(token->isNode(), "Only nodes can be parts of an operatorNode node.");
                gbiAssert(std::dynamic_pointer_cast<Node>(token), "Mismatched types, expecting Node, got something else.");

                operatorNode->push(std::static_pointer_cast<Node>(token));

                token = m_Tokens.top();
                m_Tokens.pop();
            }

            gbiAssert(
                std::dynamic_pointer_cast<Signal>(token) &&
                (std::dynamic_pointer_cast<Signal>(token)->m_Type == T::AssociatedStartSignal),
                "Expecting AdditionStart signal, got something else");

            token = m_Tokens.top();
            m_Tokens.pop();

            gbiAssert(std::dynamic_pointer_cast<Node>(token), "Expecting Node, got something else");

            operatorNode->push(std::static_pointer_cast<Node>(token));

            m_Tokens.push(operatorNode);
        }

    private:
        std::stack<std::shared_ptr<Token>> m_Tokens;
    };
}

#undef gbiAssert
