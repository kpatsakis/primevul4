local void *yarn_malloc(size_t size)
{
    return malloc_track(&mem_track, size);
}
