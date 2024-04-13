iperf_check_throttle(struct iperf_stream *sp, struct timeval *nowP)
{
    double seconds;
    uint64_t bits_per_second;

    if (sp->test->done)
        return;
    seconds = timeval_diff(&sp->result->start_time, nowP);
    bits_per_second = sp->result->bytes_sent * 8 / seconds;
    if (bits_per_second < sp->test->settings->rate) {
        sp->green_light = 1;
        FD_SET(sp->socket, &sp->test->write_set);
    } else {
        sp->green_light = 0;
        FD_CLR(sp->socket, &sp->test->write_set);
    }
}
