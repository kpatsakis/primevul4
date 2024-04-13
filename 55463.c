static int mov_metadata_int8_bypass_padding(MOVContext *c, AVIOContext *pb,
                                            unsigned len, const char *key)
{
    /* bypass padding bytes */
    avio_r8(pb);
    avio_r8(pb);
    avio_r8(pb);

    c->fc->event_flags |= AVFMT_EVENT_FLAG_METADATA_UPDATED;
    av_dict_set_int(&c->fc->metadata, key, avio_r8(pb), 0);

    return 0;
}
