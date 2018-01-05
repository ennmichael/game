#include "config_loader.h"
#include <fstream>
#include <algorithm>
#include <utility>

using namespace std::string_literals;

namespace Engine {

namespace {

std::ifstream open_file(std::string const& path)
{
        std::ifstream file(path);
        if (file.fail())
                throw Nonexistent_config_file(path);
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

Bad_config_syntax::Bad_config_syntax(std::string const& path, std::string const& line)
        : runtime_error("Syntax error in config file '"s + path + "' at line "s + line + "."s)
{}

Bad_config_value::Bad_config_value(std::string const& path, std::string const& name)
        : runtime_error("Error in config file '"s + path + "': bad value for '"s + name + "'."s)
{}

Nonexistent_config_value::Nonexistent_config_value(std::string const& path,
                                                   std::string const& name)
        : runtime_error("Error in config file '"s + path + "': value for '"s + name + "' doesn't exist."s)
{}

Nonexistent_config_file::Nonexistent_config_file(std::string const& path)
        : runtime_error("Error: config file '"s + path + "' doesn't exist."s)
{}

Config Config::load(std::string const& path)
{
        return Config(path);
}

Config::Config(std::string const& path)
        : path_(path)
{
        auto const is_whitespace =
        [](std::string const& str) noexcept
        {
                return std::all_of(str.cbegin(), str.cend(), isspace);
        };

        auto const parse_line =
        [this](std::string_view line)
        {
                auto const equals_index = line.find('=');

                if (equals_index == std::string_view::npos)
                        throw Bad_config_syntax(path_, std::string(line));

                auto const name = line.substr(0, equals_index);
                auto const value = line.substr(equals_index + 1);

                if (name == "" || value == "")
                        throw Bad_config_syntax(path_, std::string(line));

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

