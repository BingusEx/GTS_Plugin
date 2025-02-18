#pragma once 

template <typename Enum>
bool Enum_Contains(Enum value, const std::string& substring) {
    static_assert(magic_enum::detail::is_enum_v<Enum>, "Template argument must be an enum type");
    auto enum_name = std::string{ magic_enum::enum_name(value) };
    return enum_name.find(substring) != std::string::npos;
}