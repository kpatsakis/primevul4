local voidpf zlib_alloc(voidpf opaque, uInt items, uInt size)
{
    return malloc_track(opaque, items * (size_t)size);
}
