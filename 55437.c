static void asf_reset_header(AVFormatContext *s)
{
    ASFContext *asf = s->priv_data;
    ASFStream *asf_st;
    int i;

    asf->packet_size_left      = 0;
    asf->packet_flags          = 0;
    asf->packet_property       = 0;
    asf->packet_timestamp      = 0;
    asf->packet_segsizetype    = 0;
    asf->packet_segments       = 0;
    asf->packet_seq            = 0;
    asf->packet_replic_size    = 0;
    asf->packet_key_frame      = 0;
    asf->packet_padsize        = 0;
    asf->packet_frag_offset    = 0;
    asf->packet_frag_size      = 0;
    asf->packet_frag_timestamp = 0;
    asf->packet_multi_size     = 0;
    asf->packet_time_delta     = 0;
    asf->packet_time_start     = 0;

    for (i = 0; i < 128; i++) {
        asf_st = &asf->streams[i];
        av_packet_unref(&asf_st->pkt);
        asf_st->packet_obj_size = 0;
        asf_st->frag_offset = 0;
        asf_st->seq         = 0;
    }
    asf->asf_st = NULL;
}
