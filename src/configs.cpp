#include "configs.h"

using namespace std::string_literals;

namespace Game::Configs {

Mike_config::Mike_config(boost::property_tree::ptree const& tree)
        : speed(tree.get<double>("speed"s))
        , jump_speed(tree.get<double>("jump_speed"s))
        , starting_position(tree.get<double>("x"s),tree.get<double>("y"s))
        , dimensions{tree.get<int>("width"), tree.get<int>("height")}
{}

}

