#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftbitmap.h>
#include <freetype/ftoutln.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// -1 0 1
FT_Pos rand_sign(void)
{
    return rand()%3 - 1;
}

#define SPRITE_CHAR_WIDTH (300 / 2)
#define SPRITE_CHAR_HEIGHT (380 / 2)
#define VARIANTS_COUNT 3
#define ATLAS_WIDTH (SPRITE_CHAR_WIDTH*11)
#define ATLAS_HEIGHT (SPRITE_CHAR_HEIGHT*VARIANTS_COUNT)

uint32_t atlas[ATLAS_WIDTH*ATLAS_HEIGHT] = {0};
char symbols[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':' };

int main()
{
    srand(time(0));
    FT_Library library = {0};
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        fprintf(stderr, "ERROR: Could not initialize FreeType2 library\n");
        return 1;
    }

    // const char *font_file_path = "ComicMono-Bold.ttf";
    const char *font_file_path = "iosevka-bold.ttf";

    FT_Face face = {0};
    error = FT_New_Face(library, font_file_path, 0, &face);
    if (error == FT_Err_Unknown_File_Format) {
        fprintf(stderr, "ERROR: `%s` has an unknown format\n", font_file_path);
        return 1;
    } else if (error) {
        fprintf(stderr, "ERROR: Could not load file `%s`\n", font_file_path);
        return 1;
    }

    // FT_Set_Pixel_Sizes(face, 0, 500);
    FT_Set_Pixel_Sizes(face, SPRITE_CHAR_WIDTH, SPRITE_CHAR_HEIGHT);

    for (size_t variant = 0; variant < VARIANTS_COUNT; ++variant) {
        for (size_t symbol = 0; symbol < sizeof(symbols); ++symbol) {
            error = FT_Load_Char(face, symbols[symbol], FT_LOAD_DEFAULT);
            if (error) {
                fprintf(stderr, "ERROR: could not load glyph\n");
                return 1;
            }

            for (size_t i = 0; i < face->glyph->outline.n_points; ++i) {
                FT_Vector p = face->glyph->outline.points[i];
                int fuck_factor = 1;
                p.x = p.x + rand_sign()*64*fuck_factor;
                p.y = p.y + rand_sign()*64*fuck_factor;
                face->glyph->outline.points[i] = p;
            }

            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            if (error) {
                fprintf(stderr, "ERROR: could not render glyph\n");
                return 1;
            }
            assert(face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
            assert(face->glyph->bitmap.num_grays == 256);
            assert(face->glyph->bitmap.width <= SPRITE_CHAR_WIDTH);
            assert(face->glyph->bitmap.rows <= SPRITE_CHAR_HEIGHT);

            size_t atlas_x = symbol*SPRITE_CHAR_WIDTH + SPRITE_CHAR_WIDTH/2 - face->glyph->bitmap.width/2;
            size_t atlas_y = variant*SPRITE_CHAR_HEIGHT + SPRITE_CHAR_HEIGHT/2 - face->glyph->bitmap.rows/2;
            for (size_t y = 0; y < face->glyph->bitmap.rows; ++y) {
                for (size_t x = 0; x < face->glyph->bitmap.width; ++x) {
                    size_t i = (y + atlas_y)*ATLAS_WIDTH + (x + atlas_x);
                    size_t j = y*face->glyph->bitmap.pitch + x;
                    uint8_t c = face->glyph->bitmap.buffer[j];
                    atlas[i] = (c<<(3*8))|0x00FFFFFF;
                }
            }
        }
    }

    stbi_write_png("digits.png", ATLAS_WIDTH, ATLAS_HEIGHT, 4, atlas, ATLAS_WIDTH*sizeof(atlas[0]));


  // FT_Set_Char_Size( FT_Face     face,
  //                   FT_F26Dot6  char_width,
  //                   FT_F26Dot6  char_height,
  //                   FT_UInt     horz_resolution,
  //                   FT_UInt     vert_resolution );

  // FT_Outline_Copy( const FT_Outline*  source,
  //                  FT_Outline        *target );

    // error = FT_Outline_Get_Bitmap(library, &face->glyph->outline, &abitmap);
    // if (error) {
    //     fprintf(stderr, "ERROR: could not render the glyph\n");
    //     return 1;
    // }

    printf("OKI\n");

    return 0;
}
