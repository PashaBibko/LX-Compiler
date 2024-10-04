#ifndef UTIL_TRANS_TABLE_H
#define UTIL_TRANS_TABLE_H

// Includes relevant libraries for the class

#include <initializer_list>
#include <unordered_map>
#include <type_traits>
#include <utility>
#include <vector>

// Trabslation table (for translating between two types)

template<typename TypeA, typename TypeB>
class TransTable
{
    private:
        // The table
        std::unordered_map<TypeA, TypeB> aToB;
        std::unordered_map<TypeB, TypeA> bToA;

        // Checks types are not the same
        static_assert(!std::is_same<TypeA, TypeB>::value, "TypeA and TypeB cannot be the same");

    public:
        // Node class for easier parsing to the table
        struct Node
        {
            // Contents of the node
            TypeA a;
            TypeB b;

            // Constructors
            Node() : a(TypeA()), b(TypeB()) {}

            constexpr Node(TypeA a, TypeB b) : a(a), b(b) {}
        };

        // Translates from A to B
        TypeB operator[](const TypeA& a) const { return aToB.at(a); }

        // Translates from B to A
        TypeA operator[](const TypeB& b) const { return bToA.at(b); }

        // Constructors

        // Default
        TransTable() {}

        // Copy
        TransTable(const TransTable<TypeA, TypeB>& table) : aToB(table.aToB), bToA(table.bToA) {}

        // From vector of nodes
        TransTable(const std::vector<Node>& nodes)
        {
            for (Node node : nodes)
            {
                aToB[node.a] = node.b;
                bToA[node.b] = node.a;
            }
        }
        
        // Adding functions
        bool add(const TypeA& a, const TypeB& b)
        {
            if (aToB.find(a) != aToB.end() || bToA.find(b) != bToA.end())
            {
                return false;
            }

            aToB[a] = b;
            bToA[b] = a;

            return true;
        }

        bool add(const Node& node) { return add(node.a, node.b); }
};

#endif // UTIL_TRANS_TABLE_H