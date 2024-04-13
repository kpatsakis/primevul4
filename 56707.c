int ff_rtmp_check_alloc_array(RTMPPacket **prev_pkt, int *nb_prev_pkt,
                              int channel)
{
    int nb_alloc;
    RTMPPacket *ptr;
    if (channel < *nb_prev_pkt)
        return 0;

    nb_alloc = channel + 16;
    ptr = av_realloc_array(*prev_pkt, nb_alloc, sizeof(**prev_pkt));
    if (!ptr)
        return AVERROR(ENOMEM);
    memset(ptr + *nb_prev_pkt, 0, (nb_alloc - *nb_prev_pkt) * sizeof(*ptr));
    *prev_pkt = ptr;
    *nb_prev_pkt = nb_alloc;
    return 0;
}
