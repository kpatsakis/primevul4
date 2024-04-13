local void log_free(void)
{
    struct log *me;

    if (log_tail != NULL) {
#ifndef NOTHREAD
        possess(log_lock);
#endif
        while ((me = log_head) != NULL) {
            log_head = me->next;
            FREE(me->msg);
            FREE(me);
        }
#ifndef NOTHREAD
        twist(log_lock, TO, 0);
        free_lock(log_lock);
        log_lock = NULL;
        yarn_mem(malloc, free);
        free_lock(mem_track.lock);
#endif
        log_tail = NULL;
    }
}
