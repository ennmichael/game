#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <utility>
#include <type_traits>

namespace Engine {

class String_conversion_failed;

namespace {

template <class T, class Underlying>
T convert_string_impl(std::string const& str)
{
        Underlying result;
        std::stringstream stream(str);
        stream >> result;
        
        if (stream.fail() || !stream.eof())
                throw String_conversion_failed();

        return T {result};
}

}

class String_conversion_failed : public std::exception {};

template <class T>
T convert_string(std::string const& str)
{
        if constexpr (std::is_enum_v<T>)
                return convert_string_impl<T, std::underlying_type_t<T>>(str);
        else
                return convert_string_impl<T, T>(str);
}

class Bad_config_syntax : public std::runtime_error {
public:
        Bad_config_syntax(std::string const& path, std::string const& line);
};

class Bad_config_value : public std::runtime_error {
public:
        Bad_config_value(std::string const& path, std::string const& name);
};

class Nonexistent_config_value : public std::runtime_error {
public:
        Nonexistent_config_value(std::string const& path, std::string const& name);
};

class Nonexistent_config_file : public std::runtime_error {
public:
        explicit Nonexistent_config_file(std::string const& path);
};

class Config {
public:
        explicit Config(std::string const& path);

        template <class T>
        T value(std::string const& name) const
        {
                try {
                        auto const& value = values_.at(name);
                        return convert_string<T>(value);
                } catch (std::out_of_range const&) {
                        throw Nonexistent_config_value(path_, name);
                } catch (String_conversion_failed const&) {
                        throw Bad_config_value(path_, name);
                }
        }

        template <class T>
        void unpack_value(std::string const& name, T& output) const
        {
                output = value<T>(name);
        }

private:
        std::string path_;
        std::unordered_map<std::string, std::string> values_;
};

}

