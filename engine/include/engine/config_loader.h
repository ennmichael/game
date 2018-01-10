#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
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
        
        if (stream.fail() || !stream.eof())
                throw Conversion_failed();

        return result;
}

} // Close unnamed namespace

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
                } catch (Conversion_failed const&) {
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

