local void finish_jobs(void)
{
    struct job job;
    int caught;

    /* only do this once */
    if (compress_have == NULL)
        return;

    /* command all of the extant compress threads to return */
    possess(compress_have);
    job.seq = -1;
    job.next = NULL;
    compress_head = &job;
    compress_tail = &(job.next);
    twist(compress_have, BY, +1);       /* will wake them all up */

    /* join all of the compress threads, verify they all came back */
    caught = join_all();
    Trace(("-- joined %d compress threads", caught));
    assert(caught == cthreads);
    cthreads = 0;

    /* free the resources */
    caught = free_pool(&lens_pool);
    Trace(("-- freed %d block lengths buffers", caught));
    caught = free_pool(&dict_pool);
    Trace(("-- freed %d dictionary buffers", caught));
    caught = free_pool(&out_pool);
    Trace(("-- freed %d output buffers", caught));
    caught = free_pool(&in_pool);
    Trace(("-- freed %d input buffers", caught));
    free_lock(write_first);
    free_lock(compress_have);
    compress_have = NULL;
}
