#include "png++.h"
#include "compatibility.h"

namespace Png::Chunk_handling {

namespace {

unsigned char* as_characters(std::byte* bytes_ptr)
{
        return reinterpret_cast<unsigned char*>(bytes_ptr);
}

unsigned char const* as_characters(std::byte const* bytes_ptr)
{
        return reinterpret_cast<unsigned char const*>(bytes_ptr);
}

std::byte* as_bytes(unsigned char* characters_ptr)
{
        return reinterpret_cast<std::byte*>(characters_ptr);
}

std::byte const* as_bytes(unsigned char const* characters_ptr)
{
        return reinterpret_cast<std::byte const*>(characters_ptr);
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

Unique_png open_unique_png(std::string const& path)
{
        Unique_file file(std::fopen(path.c_str(), "rb"));

        if (!file)
                throw Invalid_png_file();

        Unique_png png(png_create_read_struct(PNG_LIBPNG_VER_STRING, 
                                              nullptr, nullptr, nullptr));

        if (!png)
               throw std::bad_alloc();

        png_init_io(png.get(), file.get());
        return png;
}

Chunks read_nonstandard_chunks(png_struct& png)
{
        // Enter "libpng has a garbage API"

        static Chunks chunks; // `static` to make it accessible from the callback
        chunks = {};          // Allows the function to be reentrant

        png_set_read_user_chunk_fn(&png, nullptr,
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

void write_nonstandard_chunks(Chunks& chunks, png_struct& png)
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

                png_set_unknown_chunks(&png, nullptr, &png_chunk, 1);
        }

        png_write_end(&png, nullptr);
}

}

