local void log_dump(void)
{
    if (log_tail == NULL)
        return;
    while (log_show())
        ;
    log_free();
    if (mem_track.num || mem_track.size)
        complain("memory leak: %lu allocs of %lu bytes total",
                 mem_track.num, mem_track.size);
    if (mem_track.max)
        fprintf(stderr, "%lu bytes of memory used\n", mem_track.max);
}
