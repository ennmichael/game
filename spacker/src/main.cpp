#include "spacker.h"
#include "utils.h"
#include "boost/range/irange.hpp"
#include "boost/range/adaptors.hpp"
#include "boost/program_options.hpp"
#include "boost/variant.hpp"
#include <string_view>
#include <exception>
#include <sstream>
#include <optional>
#include <iostream>

/**
 * NOTE:
 * As spacker is being written, the sprite database currently contains
 * files that seem to be of approximately the same height.
 * The algorithm is designed to work for this case only.
 */

namespace {

namespace gil = boost::gil;
namespace po = boost::program_options;

using namespace std::string_literals;

std::string description_text(po::options_description const& desc)
{
        std::stringstream ss;
        ss << desc;
        return ss.str();
}

struct CommandLineOptions {
        using HelpString = std::string;

        static boost::variant<HelpString, CommandLineOptions> parse(int argc, char** argv)
        {
                auto const desc = options_description();
                auto const vm = variables_map(po::command_line_parser(argc, argv)
                                                     .options(desc)
                                                     .positional(positional_options_description())
                                                     .run());
                
                if (vm.count("help"s)
                    || !vm.count("output-file"s)
                    || !vm.count("input-files"s))
                        return description_text(desc);

                return CommandLineOptions {
                        vm["output-file"s].as<std::string>(),
                        vm["input-files"s].as<std::vector<std::string>>()
                };
        }

        std::string output_path;
        std::vector<std::string> input_paths;

private:
        template <class ParsedOptions>
        static po::variables_map variables_map(ParsedOptions const& parsed_options)
        {
                po::variables_map vm;
                po::store(parsed_options, vm);
                return vm;
        } 

        static po::positional_options_description positional_options_description()
        {
                po::positional_options_description desc;
                desc.add("input-files", -1);
                return desc;
        }

        static po::options_description options_description()
        {
                po::options_description desc("Options");
                desc.add_options()
                        ("help", "show help screen")

                        ("output-file,o",
                         po::value<std::string>(),
                         "specify base name for output files")

                        ("input-files",
                         po::value<std::vector<std::string>>(),
                         "specify input files");

                return desc;
        }
};

}

int main(int argc, char **argv)
{
        struct CommandLineVisitor : boost::static_visitor<void> {
                void operator()(CommandLineOptions::HelpString const& help) const
                {
                        std::cout << help << '\n';
                }

                void operator()(CommandLineOptions const& options) const
                {
                        auto const sprites = Spacker::load_sprites(options.input_paths);
                        auto const sprite_sheet = Spacker::SpriteSheet::pack(sprites);
                        sprite_sheet.write_to(options.output_path);
                }
        };

        auto const parsed_result = CommandLineOptions::parse(argc, argv);
        boost::apply_visitor(
                CommandLineVisitor(),
                parsed_result);
}

