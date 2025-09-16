#include "ReflectionUtility.hpp"
#include <vector>
#include "TypeUtility.hpp"

using namespace LittleCore;

struct Child {
    std::string name;
    int age;
    int yearBorn=1995;
};

struct User {
    int id{};
    std::string name{};
    double score{};
    std::vector<int> indices;
    std::vector<Child> children;
    Child root;
};

int main() {
    User u{1, "Alice", 42.0};
    u.indices = {1,2,3,4};
    u.children = {{"Jeppe", 43}, {"Ella", 9}, {"Aksel", 5}};

    ReflectionUtility::IterateMembers(u, [](const auto& fieldName, auto& value) {
        //std::cout << fieldName << " -> "<< value << " " << LittleCore::TypeUtility::GetClassName<decltype(value)>() <<  "\n";
        using type = std::remove_cvref_t<decltype(value)>;

        std::cout << fieldName << " -> "<< TypeUtility::GetClassName<type>() <<  "\n";
    });

}
