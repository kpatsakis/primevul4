local void *malloc_track(struct mem_track_s *mem, size_t size)
{
    void *ptr;

    ptr = malloc(size);
    if (ptr != NULL) {
        size = MALLOC_SIZE(ptr);
        mem_track_grab(mem);
        mem->num++;
        mem->size += size;
        if (mem->size > mem->max)
            mem->max = mem->size;
        mem_track_drop(mem);
    }
    return ptr;
}
