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

namespace Png {

using Bytes = std::vector<std::byte>;
using Chunks = std::unordered_map<std::string, Bytes>;

class Cant_open_file : public std::exception {};

class Png_struct_deleter {
public:
        void operator()(png_struct* png);
};

class Png_info_deleter {
public:
        void operator()(png_info* info);
};

using Unique_png_struct = std::unique_ptr<png_struct, Png_struct_deleter>;
using Unique_png_info = std::unique_ptr<png_info, Png_info_deleter>;

class Png_file {
public:
        static Png_file open(std::string const& path);

        explicit Png_file(Unique_png_struct png_struct) noexcept;

        Chunks read_nonstandard_chunks();
        void write_nonstandard_chunks(Chunks& chunks);

private:
        Unique_png_struct struct_;
        Unique_png_info info_;
};

}

