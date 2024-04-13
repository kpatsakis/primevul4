void ff_rm_free_rmstream (RMStream *rms)
{
    av_packet_unref(&rms->pkt);
}
