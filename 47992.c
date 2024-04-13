local void zlib_free(voidpf opaque, voidpf address)
{
    free_track(opaque, address);
}
