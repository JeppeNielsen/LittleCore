#include <glaze/glaze.hpp>
#include <glaze/concepts/container_concepts.hpp>
#include <iostream>
#include <utility>
#include "TupleHelper.hpp"
#include <concepts>
#include "TypeUtility.hpp"

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

template <typename T>
concept StringLike =
    std::same_as<std::decay_t<T>, std::string> ||
    std::same_as<std::decay_t<T>, std::string_view> ||
    std::is_same_v<std::remove_cvref_t<T>, char*> ||
    std::is_same_v<std::remove_cvref_t<T>, const char*>;

template <typename T>
concept SimpleType = std::is_arithmetic_v<T> || std::is_enum_v<T>;

template<typename T, typename Callable>
void IterateMembers(T&& object, Callable&& callable) {

    decltype(glz::reflect<T>()) fieldNames;

    int memberIndex = 0;
    glz::for_each_field(object, [&, callable](auto& value) {
        using valueType = std::remove_cvref_t<decltype(value)>;

        if constexpr (StringLike<valueType>) {
            callable(fieldNames.keys[memberIndex], value);

        } else if constexpr (SimpleType<valueType>) {
            callable(fieldNames.keys[memberIndex], value);
        } else if constexpr (glz::detail::vector_like<valueType>) {

            for(auto& v : value) {
                using containedType = std::remove_cvref_t<decltype(v)>;

                if constexpr (SimpleType<containedType>) {
                    callable(fieldNames.keys[memberIndex], v);
                } else {
                    IterateMembers<decltype(v)>(v, callable);
                }
            }

        } else {
            callable(fieldNames.keys[memberIndex], value);
            IterateMembers(value, callable);
        }
        memberIndex++;
    });

}

int main() {
    User u{1, "Alice", 42.0};
    u.indices = {1,2,3,4};
    u.children = {{"Jeppe", 43}, {"Ella", 9}, {"Aksel", 5}};

    IterateMembers(u, [](const auto& fieldName, auto& value) {
        //std::cout << fieldName << " -> "<< value << " " << LittleCore::TypeUtility::GetClassName<decltype(value)>() <<  "\n";
        using type = std::remove_cvref_t<decltype(value)>;

        std::cout << fieldName << " -> "<< LittleCore::TypeUtility::GetClassName<type>() <<  "\n";
    });

}
