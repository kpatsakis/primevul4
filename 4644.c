} check_gzip_header(unsigned char const *data, ssize_t len, ssize_t *headerlen)
{
  int method, flags;
  const ssize_t totallen = len;

  /* The shortest header is 10 bytes */
  if(len < 10)
    return GZIP_UNDERFLOW;

  if((data[0] != GZIP_MAGIC_0) || (data[1] != GZIP_MAGIC_1))
    return GZIP_BAD;

  method = data[2];
  flags = data[3];

  if(method != Z_DEFLATED || (flags & RESERVED) != 0) {
    /* Can't handle this compression method or unknown flag */
    return GZIP_BAD;
  }

  /* Skip over time, xflags, OS code and all previous bytes */
  len -= 10;
  data += 10;

  if(flags & EXTRA_FIELD) {
    ssize_t extra_len;

    if(len < 2)
      return GZIP_UNDERFLOW;

    extra_len = (data[1] << 8) | data[0];

    if(len < (extra_len + 2))
      return GZIP_UNDERFLOW;

    len -= (extra_len + 2);
    data += (extra_len + 2);
  }

  if(flags & ORIG_NAME) {
    /* Skip over NUL-terminated file name */
    while(len && *data) {
      --len;
      ++data;
    }
    if(!len || *data)
      return GZIP_UNDERFLOW;

    /* Skip over the NUL */
    --len;
    ++data;
  }

  if(flags & COMMENT) {
    /* Skip over NUL-terminated comment */
    while(len && *data) {
      --len;
      ++data;
    }
    if(!len || *data)
      return GZIP_UNDERFLOW;

    /* Skip over the NUL */
    --len;
  }

  if(flags & HEAD_CRC) {
    if(len < 2)
      return GZIP_UNDERFLOW;

    len -= 2;
  }

  *headerlen = totallen - len;
  return GZIP_OK;
}