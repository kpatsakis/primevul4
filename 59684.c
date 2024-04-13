resolve_string_offset (void    *buffer,
                       size_t   buffer_size,
                       uint32_t offset)
{
  if (offset == 0)
    return NULL;

  if (offset > buffer_size)
    die ("Invalid string offset %d (buffer size %zd)", offset, buffer_size);

  return (const char *) buffer + offset;
}
