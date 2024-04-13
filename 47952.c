local void free_track(struct mem_track_s *mem, void *ptr)
{
    size_t size;

    if (ptr != NULL) {
        size = MALLOC_SIZE(ptr);
        mem_track_grab(mem);
        mem->num--;
        mem->size -= size;
        mem_track_drop(mem);
        free(ptr);
    }
}
