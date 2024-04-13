fill_input_buffer (j_decompress_ptr cinfo)
{
  static uchar jpeg_buffer[4096];
  size_t nbytes;
  DCRaw *d = (DCRaw*)cinfo->client_data;

  nbytes = fread (jpeg_buffer, 1, 4096, d->ifp);
  swab ((const char *)jpeg_buffer, (char *)jpeg_buffer, nbytes); /*mingw UF*/
  cinfo->src->next_input_byte = jpeg_buffer;
  cinfo->src->bytes_in_buffer = nbytes;
  return TRUE;
}
