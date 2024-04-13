local void setup_jobs(void)
{
    /* set up only if not already set up*/
    if (compress_have != NULL)
        return;

    /* allocate locks and initialize lists */
    compress_have = new_lock(0);
    compress_head = NULL;
    compress_tail = &compress_head;
    write_first = new_lock(-1);
    write_head = NULL;

    /* initialize buffer pools (initial size for out_pool not critical, since
       buffers will be grown in size if needed -- initial size chosen to make
       this unlikely -- same for lens_pool) */
    new_pool(&in_pool, g.block, INBUFS(g.procs));
    new_pool(&out_pool, OUTPOOL(g.block), -1);
    new_pool(&dict_pool, DICT, -1);
    new_pool(&lens_pool, g.block >> (RSYNCBITS - 1), -1);
}
