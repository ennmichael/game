#include "png++/png++.h"
#include <vector>
#include <string>

using namespace std::string_literals;

auto translate_args(int argc, char** argv)
{
        return std::vector<std::string>(argv, argv + argc);
}

int main(int argc, char** argv)
{
        std::vector<std::string> const args = translate_args(argc, argv);

        Png::Chunks chunks {
                {"Example1"s, Png::Bytes{std::byte(1), std::byte(2)}},
                {"Example2"s, Png::Bytes{std::byte(3), std::byte(4)}}
        };

        auto png = Png::open_file(args[0]);

        Png::write_nonstandard_chunks(*png, chunks);
}

