#include "spacker.h"
#include "utils.h"
#include "boost/range/irange.hpp"
#include "boost/range/adaptors.hpp"

// TODO Rename *.hpp -> *.h

/**
 * NOTE:
 * As spacker is being written, the sprite database currently contains
 * files that seem to be of approximately the same height, and this has
 * very much influenced the design of the packing algorithm.
 * The algorithm prefers stacking the sprites horizontally.
 */

namespace {

namespace gil = boost::gil;

auto parameters_vector(int argc, char** argv)
{
        return Spacker::Utils::range_to_vector(
                boost::irange(1, argc)
              | boost::adaptors::transformed([&](auto const i) -> char const*
                                             { return argv[i]; }));
}

}

int main(int argc, char **argv)
{
        if (argc == 1)
                return 0;

        auto const parameters = parameters_vector(argc, argv);
        auto const sprites = Spacker::load_images(parameters);
        auto const sprite_sheet = Spacker::SpriteSheet::pack(sprites);
        sprite_sheet.write_to("./out");
}

