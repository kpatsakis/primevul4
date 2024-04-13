local void log_init(void)
{
    if (log_tail == NULL) {
        mem_track.num = 0;
        mem_track.size = 0;
        mem_track.max = 0;
#ifndef NOTHREAD
        mem_track.lock = new_lock(0);
        yarn_mem(yarn_malloc, yarn_free);
        log_lock = new_lock(0);
#endif
        log_head = NULL;
        log_tail = &log_head;
    }
}
