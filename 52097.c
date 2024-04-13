diskfile_send(struct iperf_stream *sp)
{
    int r;

    r = read(sp->diskfile_fd, sp->buffer, sp->test->settings->blksize);
    if (r == 0)
        sp->test->done = 1;
    else
	r = sp->snd2(sp);
    return r;
}
