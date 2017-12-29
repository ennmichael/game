#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <exception>

namespace Engine {

using Config_value = std::variant<int, std::string>;
using Config = std::unordered_map<std::string, Config_value >;

// This exception should carry a lot more information in production
class Invalid_config_syntax : public std::exception {};

Config load_config_file(std::string const& path);

}

