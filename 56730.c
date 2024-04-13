static void png_get_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  Image
    *image;

  image=(Image *) png_get_io_ptr(png_ptr);
  if (length != 0)
    {
      png_size_t
        check;

      check=(png_size_t) ReadBlob(image,(size_t) length,data);
      if (check != length)
        {
          char
            msg[MagickPathExtent];

          (void) FormatLocaleString(msg,MagickPathExtent,
            "Expected %.20g bytes; found %.20g bytes",(double) length,
            (double) check);
          png_warning(png_ptr,msg);
          png_error(png_ptr,"Read Exception");
        }
    }
}
