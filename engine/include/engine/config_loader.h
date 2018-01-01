#pragma once

#include <unordered_map>
#include <string>
#include <exception>
#include <sstream>

namespace Engine {

namespace {

class Conversion_failed : public std::exception {};

template <class T>
T convert_string(std::string str)
{
        T result;
        std::stringstream stream(str);
        stream >> result;
        
        if (stream.fail())
                throw Conversion_failed();

        return result;
}

} // Close unnamed namespace

// TODO These should carry more information at production

class Bad_config_syntax : public std::exception {};
class Bad_config_value : public std::exception {};
class Nonexistent_config_value : public std::exception {};

class Config {
public:
        static Config load_from_file(std::string const& path);

        explicit Config(std::string const& path);

        template <class T>
        T value(std::string const& name) const
        {
                try {
                        auto const& value = values_.at(name);
                        return convert_string<T>(value);
                } catch (std::out_of_range const&) {
                        throw Nonexistent_config_value();
                } catch (Conversion_failed const&) {
                        throw Bad_config_value();
                }
        }

        template <class T>
        T unpack_value(std::string const& name, T& output) const
        {
                output = value<T>(name);
        }

private:
        std::unordered_map<std::string, std::string> values_;
};

class Nonexistent_config_file : public std::exception {};

}

