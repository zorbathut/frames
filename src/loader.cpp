
#include "frames/loader.h"

#include <stdlib.h>
#include <stdio.h>
extern "C" {
#include "jpeglib.h"
}
#include <png.h>

#include "frames/configuration.h"
#include "frames/environment.h"
#include "frames/stream.h"
#include "frames/texture.h"

#include <vector>

namespace Frames {
  bool Loader::PNG::Is(const StreamPtr &stream) {
    unsigned char tag[8];
    int64_t got = stream->Read(tag, 8);
    bool rv = false;
    if (got == 8) {
      rv = !png_sig_cmp(tag, 0, 8);
    }
    stream->Seek(0);
    return rv;
  }

  void PngReader(png_structp png_ptr, png_bytep data, png_size_t length) {
    Stream *istr = (Stream*)png_get_io_ptr(png_ptr);
    
    int64_t ct = istr->Read(data, (int)length); // if you have a 2gb png, at least this means you won't read all of it
    if(ct != length)
      png_error(png_ptr, "unexpected EOF");
  }

  TexturePtr Loader::PNG::Load(Environment *env, const StreamPtr &stream) {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
      return TexturePtr();
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      return TexturePtr();
    }

    TexturePtr tinfo;  // tinfo created here so its destructor will trigger if the longjmp is hit
    std::vector<unsigned char *> ul;
    
    if(setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return TexturePtr();
    }
    
    png_set_read_fn(png_ptr, stream.Get(), PngReader);
    
    png_read_info(png_ptr, info_ptr);
    
    if(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE) {
      png_set_palette_to_rgb(png_ptr);
    }
    if(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY && png_get_bit_depth(png_ptr, info_ptr) < 8) {
      png_set_expand_gray_1_2_4_to_8(png_ptr);
    }
    if(png_get_bit_depth(png_ptr, info_ptr) == 16) {
      png_set_strip_16(png_ptr);
    }
    if(png_get_bit_depth(png_ptr, info_ptr) < 8) {
      png_set_packing(png_ptr);
    }
    if(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY || png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY_ALPHA) {
      png_set_gray_to_rgb(png_ptr);
    }
    if(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB || png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY) {
      png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    }
    
    png_read_update_info(png_ptr, info_ptr);
    
    if (png_get_bit_depth(png_ptr, info_ptr) != 8 ||
        !(png_get_channels(png_ptr, info_ptr) == 4 || png_get_channels(png_ptr, info_ptr) == 3) ||
        !(png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGBA || png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return TexturePtr();
    }

    tinfo = Texture::CreateRawManaged(env, png_get_image_width(png_ptr, info_ptr), png_get_image_height(png_ptr, info_ptr), Texture::FORMAT_RGBA_8);
    
    for(int i = 0; i < (int)png_get_image_height(png_ptr, info_ptr); ++i)
      ul.push_back(tinfo->RawDataGet() + i * tinfo->WidthGet() * 4);
    png_read_image(png_ptr, (png_byte**)&ul[0]);
        
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return tinfo;
  }

  bool Loader::JPG::Is(const StreamPtr &stream) {
    unsigned char tag[3];
    int64_t got = stream->Read(tag, 3);
    bool rv = false;
    if (got == 3) {
      rv =  (tag[0] == 0xFF && tag[1] == 0xD8 && tag[2] == 0xFF);
    }
    stream->Seek(0);
    return rv;
  }

  namespace detail {
    struct JpegErrorManager {
      struct jpeg_error_mgr pub; // built-in functionality

      Environment *env;
      jmp_buf setjmp_buffer;	// return to us
    };
    void JpegError(j_common_ptr info) {
      JpegErrorManager *jerr = (JpegErrorManager*)info->err;

      longjmp(jerr->setjmp_buffer, 1);
    }
    void JpegMessage(j_common_ptr info) {
      JpegErrorManager *jerr = (JpegErrorManager*)info->err;

      char buffer[JMSG_LENGTH_MAX];

      /* Create the message */
      (*info->err->format_message)(info, buffer);

      jerr->env->LogDebug(buffer);
    }

    const int JPEG_BUFFER_SIZE = 4096;
    struct JpegBufferManager {
      struct jpeg_source_mgr pub;

      Stream *stream;
      JOCTET buffer[JPEG_BUFFER_SIZE]; // start of buffer
    };
    void JpegInit(j_decompress_ptr info) {
      // no-op, we're already initted
    }
    boolean JpegFill(j_decompress_ptr info) {
      JpegBufferManager *jerr = (JpegBufferManager*)info->src;

      jerr->pub.next_input_byte = jerr->buffer;
      jerr->pub.bytes_in_buffer = (size_t)jerr->stream->Read(jerr->buffer, JPEG_BUFFER_SIZE); // not really concerned about 5gb jpgs here

      return TRUE;
    }
    void JpegSkip(j_decompress_ptr cinfo, long num_bytes) {
      struct jpeg_source_mgr * src = cinfo->src;

      /* Just a dumb implementation for now.  Could use fseek() except
       * it doesn't work on pipes.  Not clear that being smart is worth
       * any trouble anyway --- large skips are infrequent.
       */
      if (num_bytes > 0) {
        while (num_bytes > (long) src->bytes_in_buffer) {
          num_bytes -= (long) src->bytes_in_buffer;
          (void) (*src->fill_input_buffer) (cinfo);
          /* note we assume that fill_input_buffer will never return FALSE,
           * so suspension need not be handled.
           */
        }
        src->next_input_byte += (size_t) num_bytes;
        src->bytes_in_buffer -= (size_t) num_bytes;
      }
    }
    void JpegTerminate(j_decompress_ptr info) {
      // no-op, we don't terminate
    }

    void JpegMemorySource(j_decompress_ptr info, const unsigned char *buffer, int buffer_size) {}
  }

  TexturePtr Loader::JPG::Load(Environment *env, const StreamPtr &stream) {
    TexturePtr rv; // rv created here to deallocate if we hit the setjmp
    jpeg_decompress_struct info;

    // Set up the error handler. If jpeglib runs into an error at any future point, it will
    // execute the block after setjmp(), which will otherwise remain unexecuted.
    detail::JpegErrorManager jerr;
    info.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = detail::JpegError;
    jerr.pub.output_message = detail::JpegMessage;
    jerr.env = env;
    if (setjmp(jerr.setjmp_buffer)) {
      jpeg_destroy_decompress(&info);
      return TexturePtr();
    }
    
    // Set up the memory source
    jpeg_create_decompress(&info);

    // Set up the buffer manager.
    detail::JpegBufferManager buffer;
    info.src = &buffer.pub;
    buffer.pub.init_source = detail::JpegInit;
    buffer.pub.fill_input_buffer = detail::JpegFill;
    buffer.pub.skip_input_data = detail::JpegSkip;
    buffer.pub.resync_to_restart = jpeg_resync_to_restart;
    buffer.pub.term_source = detail::JpegTerminate;
    buffer.pub.next_input_byte = 0;
    buffer.pub.bytes_in_buffer = 0;
    buffer.stream = stream.Get();

    // Decompress the jpg image
  	jpeg_read_header(&info, true);
    jpeg_start_decompress(&info);

    rv = Texture::CreateRawManaged(env, info.image_width, info.image_height, (info.num_components == 3 ? Texture::FORMAT_RGB_8 : Texture::FORMAT_L_8));

    for (int y = 0; y < (int)info.image_height; ++y) {
      unsigned char *row = rv->RawDataGet() + y * rv->RawStrideGet();
      if (jpeg_read_scanlines(&info, &row, 1) != 1) {
        jpeg_destroy_decompress(&info);
        return TexturePtr();
      }
    }
    
    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);

    return rv;
  }
}
