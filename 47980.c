local void *realloc_track(struct mem_track_s *mem, void *ptr, size_t size)
{
    size_t was;

    if (ptr == NULL)
        return malloc_track(mem, size);
    was = MALLOC_SIZE(ptr);
    ptr = realloc(ptr, size);
    if (ptr != NULL) {
        size = MALLOC_SIZE(ptr);
        mem_track_grab(mem);
        mem->size -= was;
        mem->size += size;
        if (mem->size > mem->max)
            mem->max = mem->size;
        mem_track_drop(mem);
    }
    return ptr;
}
