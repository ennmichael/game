#include "png++.h"
#include "compatibility.h"
#include <utility>







#include <iostream>




namespace Png {

namespace {

unsigned char* as_characters(std::byte* bytes_ptr)
{
        return reinterpret_cast<unsigned char*>(bytes_ptr);
}

std::byte* as_bytes(unsigned char* characters_ptr)
{
        return reinterpret_cast<std::byte*>(characters_ptr);
}

class File_deleter {
public:
        void operator()(std::FILE* file) const noexcept
        {
                std::fclose(file);
        }
};

using Unique_file = std::unique_ptr<std::FILE, File_deleter>;

} // Close anonymous namespace

void Png_struct_deleter::operator()(png_struct* png)
{
        png_destroy_read_struct(&png, nullptr, nullptr);
}

Png_file Png_file::open(std::string const& path)
{
        Unique_file file(std::fopen(path.c_str(), "rb"));

        if (!file)
                throw Cant_open_file();

        Unique_png_struct png_struct(png_create_read_struct(PNG_LIBPNG_VER_STRING, 
                                                            nullptr, nullptr, nullptr));

        if (!png_struct)
               throw std::bad_alloc();

        png_init_io(png_struct.get(), file.get());
        return Png_file(std::move(png_struct));
}

Png_file::Png_file(Unique_png_struct png_struct, Unique_png_info png_info) noexcept
        : struct_(std::move(png_struct))
        , info_(png_create_info_struct(struct_.get()))
{}

Chunks Png_file::read_nonstandard_chunks()
{
        // Enter "libpng has a garbage API"

        static Chunks chunks; // `static` to make it accessible from the callback
        chunks = {};          // Allows the function to be reentrant

        png_set_read_user_chunk_fn(struct_.get(), nullptr,
                [](auto*, png_unknown_chunk* chunk)
                {
                        std::string const name(reinterpret_cast<char const*>(chunk->name));

                        auto const data_ptr = as_bytes(chunk->data);
                        chunks[name] = Bytes(data_ptr, data_ptr + chunk->size);

                        return 1;
                }
        );

        return chunks;
}

void write_nonstandard_chunks(Chunks& chunks)
{
        for (auto& [chunk_name, chunk] : chunks) {
                png_unknown_chunk png_chunk {
                        {
                                static_cast<unsigned char>(chunk_name[0]),
                                static_cast<unsigned char>(chunk_name[1]),
                                static_cast<unsigned char>(chunk_name[2]),
                                static_cast<unsigned char>(chunk_name[3]),
                                static_cast<unsigned char>('\0')
                        },
                        as_characters(&chunk[0]),
                        chunk.size(),
                        0
                };
                
                png_set_unknown_chunks(struct_.get(), info_.get(), &png_chunk, 1);

                std::cout << "1\n";
        }

        png_write_end(struct_.get(), info_.get());

        std::cout << "2\n";
}

}

