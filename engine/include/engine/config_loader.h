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
        
        if (stream.fail())
                throw Conversion_failed();

        return result;
}

} // Close unnamed namespace

// TODO These should carry more information at production

class Bad_config_syntax : public std::runtime_error {
        using std::runtime_error::runtime_error;
};

class Bad_config_value : public std::runtime_error {
        using std::runtime_error::runtime_error;
};

class Nonexistent_config_value : public std::runtime_error {
        using std::runtime_error::runtime_error;
};


class Nonexistent_config_file : public std::runtime_error {
        using std::runtime_error::runtime_error;
};

class Config { // TODO Store the `path` too, so that we can throw proper exceptions
public:
        static Config load(std::string const& path);

        explicit Config(std::string const& path);

        template <class T>
        T value(std::string const& name) const
        {
                try {
                        auto const& value = values_.at(name);
                        return convert_string<T>(value);
                } catch (std::out_of_range const&) {
                        throw Nonexistent_config_value(name);
                } catch (Conversion_failed const&) {
                        throw Bad_config_value(name);
                }
        }

        template <class T>
        void unpack_value(std::string const& name, T& output) const
        {
                output = value<T>(name);
        }

private:
        std::unordered_map<std::string, std::string> values_;
};

}

