#include "config_loader.h"
#include <fstream>
#include <algorithm>
#include <utility>

namespace Engine {

namespace {

std::ifstream open_file(std::string const& path)
{
        std::ifstream file(path);
        if (file.fail())
                throw Nonexistent_config_file();
        return file;
}

std::string_view trim(std::string_view str, 
                      std::string_view whitespace = " \f\n\r\t\v") noexcept
{
        auto const from = str.find_first_not_of(whitespace);
        
        if (from == 0)
            return str;
            
        auto const to = str.find_last_not_of(whitespace, from + 1);
        return str.substr(from, to + 1);
}

} // Close unnamed namespace

Config Config::load_from_file(std::string const& path)
{
        return Config(path);
}

Config::Config(std::string const& path)
{
        auto const is_whitespace =
        [](std::string const& str) noexcept
        {
                return std::all_of(str.cbegin(), str.cend(), isspace);
        };

        auto const parse_line =
        [](std::string_view line)
        {
                auto const equals_index = line.find('=');

                if (equals_index == std::string_view::npos)
                        throw Bad_config_syntax();

                auto const name = line.substr(0, equals_index);
                auto const value = line.substr(equals_index + 1);

                if (name == "" || value == "")
                        throw Bad_config_syntax();

                return std::pair(name, value);
        };

        auto input = open_file(path);
        for (std::string line; std::getline(input, line);) {
                if (is_whitespace(line))
                        continue;

                auto const [name, value] = parse_line(trim(line));
                values_[std::string(name)] = value;
        }
}

}

