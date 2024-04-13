static int mov_metadata_int8_no_padding(MOVContext *c, AVIOContext *pb,
                                        unsigned len, const char *key)
{
    c->fc->event_flags |= AVFMT_EVENT_FLAG_METADATA_UPDATED;
    av_dict_set_int(&c->fc->metadata, key, avio_r8(pb), 0);

    return 0;
}
