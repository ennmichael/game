#pragma once

#include "engine/base.h"
#include "engine/utils.h"
#include "boost/property_tree/ptree.hpp"
#include <string>

namespace Game::Configs {

struct Mike_config {
        explicit Mike_config(boost::property_tree::ptree const& tree);

        double speed;
        double jump_speed;
        Engine::Complex_number starting_position;
        Engine::Dimensions dimensions;
};

template <class Config, class String>
Config load_config(String const& path)
{
        auto const tree = Engine::Utils::read_json_tree(path);
        return Config(tree);
}

struct Enemy_config {};

}

