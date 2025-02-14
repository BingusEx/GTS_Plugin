#pragma once

#include <reflect> // boost-ext/reflect
#include <toml.hpp>
#include <magic_enum/magic_enum.hpp>

/*
    TOML11 Reflection Macro & Templates. Based on https://github.com/ToruNiina/toml11/blob/main/examples/reflect/reflect.hpp
*/

namespace toml {
    namespace refl {
        template<typename T, typename TC>
        T from(const basic_value<TC>& v) {
            T x;
            reflect::for_each([&v, &x](auto I) {
                using member_type = std::remove_cvref_t<decltype(reflect::get<I>(x))>;
                const auto key = std::string(reflect::member_name<I>(x));
                //Changed to find_or. If the value is not found return the struct's value.
                reflect::get<I>(x) = toml::find_or<member_type>(v, key,reflect::get<I>(x));
            }, x);
            return x;
        }

        template<typename TC = toml::ordered_type_config, typename T>
        basic_value<TC> into(const T& x) {
            basic_value<TC> v(toml::table{});
            reflect::for_each([&v, &x](auto I) {
                using member_type = std::remove_cvref_t<decltype(reflect::get<I>(x))>;
                const auto key = std::string(reflect::member_name<I>(x));
                v[key] = reflect::get<I>(x);
            }, x);
            return v;
        }
    } // refl
} // toml

#define TOML_SERIALIZABLE(X)                               \
namespace toml {                                           \
    template<>                                             \
    struct into<X>                                         \
    {                                                      \
        template<typename TC = toml::ordered_type_config>  \
        static toml::basic_value<TC> into_toml(const X& x) \
        {                                                  \
            return refl::into(x);                          \
        }                                                  \
    };                                                     \
    template<>                                             \
    struct from<X>                                         \
    {                                                      \
        template<typename TC = toml::ordered_type_config>  \
        static X from_toml(const toml::basic_value<TC>& v) \
        {                                                  \
            return refl::from<X>(v);                       \
        }                                                  \
    };                                                     \
} /* toml */

/*
    Helper functions Used For TOML SerDe
*/

namespace GTS {

    template<typename Enum>
    [[nodiscard]] static inline Enum StringToEnum(const std::string& name) {
        auto value = magic_enum::enum_cast<Enum>(name);
        if (value.has_value()) {
            return *value;
        }
        else {
            throw std::invalid_argument("Invalid enum name: " + name);
        }
    }

    [[nodiscard]] static inline bool CheckFile(const std::filesystem::path& a_file) {
        MessageBoxA(nullptr, a_file.string().c_str(), "aaa", MB_OK);
        try {
            // Check if the file exists
            if (std::filesystem::exists(a_file)) {
                return true;
            }

            // Try to create the file
            std::ofstream file(a_file);
            file.exceptions(std::ofstream::failbit);
            if (file) {
                file.close();
                logger::warn("Configuration file did not exist but was successfully created");
                return true;
            }

        }
        catch (const std::filesystem::filesystem_error& e) {
            logger::error("CheckFile() Filesystem error: {}", e.what());
            return false;
        }
        catch (const std::exception& e) {
            logger::error("CheckFile() -> Exception: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("CheckFile() -> Unknown Exception");
            return false;
        }
    }

    [[nodiscard]] static inline bool DeleteFile(const std::filesystem::path& a_file) {
        try {
            // Check if the file exists
            if (std::filesystem::exists(a_file)) {
                std::filesystem::remove(a_file);
                logger::info("Configuration file was successfully deleted");
                return true;
            }
            return false;

        }
        catch (const std::filesystem::filesystem_error& e) {
            logger::error("DeleteFile() Filesystem error: {}", e.what());
            return false;
        }
        catch (const std::exception& e) {
            logger::error("DeleteFile() -> Exception: {}", e.what());
            return false;
        }
        catch (...) {
            logger::error("DeleteFile() -> Unknown Exception");
            return false;
        }
    }
}