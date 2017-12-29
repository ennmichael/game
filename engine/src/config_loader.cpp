#include "config_loader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

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

Config load_config_file(std::string const& path)
{
        struct Input_line {
                static Input_line parse(std::string_view line)
                {
                        auto const space_index = line.find(' ');
                        auto const equals_index = line.find('=');

                        if (space_index == std::string::npos || 
                            equals_index == std::string::npos) {
                                throw Invalid_config_syntax();
                        }

                        return {
                                line.substr(0, space_index),
                                line.substr(space_index + 1, equals_index - space_index - 1),
                                line.substr(equals_index + 1, line.size() - equals_index)
                        };
                }
                
                Config_value interpret_value() const
                {
                        if (type == "int") {
                                try {
                                        return convert_string<int>(std::string(value));
                                } catch (Conversion_failed const&) {
                                        throw Invalid_config_syntax();
                                }
                        }
                        if (type == "str") {
                                return std::string(value);
                        }
                        
                        throw Invalid_config_syntax();
                }

                std::string_view type;
                std::string_view name;
                std::string_view value;
        };

        auto const is_whitespace=
        [](std::string const& str) noexcept
        {
                return std::all_of(str.cbegin(), str.cend(), isspace);
        };

        Config config;

        std::ifstream input(path);
        for (std::string line; std::getline(input, line);) {
                if (is_whitespace(line))
                        continue;

                auto const parsed_line = Input_line::parse(line);
                auto const name = std::string(parsed_line.name);
                config[name] = parsed_line.interpret_value();
        }

        return config;
}

}

