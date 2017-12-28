#pragma once

// I happen to have named this subproject "png++".
// It has nothing to do with the other "png++" wrapper for libpng.
// TODO I should aim to rename this subproject soon.

#include "png.h"
#include <unordered_map>
#include <string>
#include <cstddef>
#include <vector>
#include <exception>
#include <memory>

namespace Png::Chunk_handling {

using Bytes = std::vector<std::byte>;
using Chunks = std::unordered_map<std::string, Bytes>;

class Invalid_png_file : public std::exception {};

class Png_struct_deleter {
public:
        void operator()(png_struct* png)
        {
                png_destroy_read_struct(&png, nullptr, nullptr);
        }
};

using Unique_png = std::unique_ptr<png_struct, Png_struct_deleter>;

Unique_png open_unique_png(std::string const& path);

Chunks read_nonstandard_chunks(png_struct& png);
void write_nonstandard_chunks(Chunks& chunks, png_struct& png);
// I am certain that the `chunks` parameter may as well be const,
// but, unfortunately, this is libpng API we're talking about.

}

