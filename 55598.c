static int nsv_read_close(AVFormatContext *s)
{
    NSVContext *nsv = s->priv_data;

    av_freep(&nsv->nsvs_file_offset);
    av_freep(&nsv->nsvs_timestamps);
    if (nsv->ahead[0].data)
        av_packet_unref(&nsv->ahead[0]);
    if (nsv->ahead[1].data)
        av_packet_unref(&nsv->ahead[1]);
    return 0;
}
