static int asf_read_ext_stream_properties(AVFormatContext *s, int64_t size)
{
    ASFContext *asf = s->priv_data;
    AVIOContext *pb = s->pb;
    ff_asf_guid g;
    int ext_len, payload_ext_ct, stream_ct, i;
    uint32_t leak_rate, stream_num;
    unsigned int stream_languageid_index;

    avio_rl64(pb); // starttime
    avio_rl64(pb); // endtime
    leak_rate = avio_rl32(pb); // leak-datarate
    avio_rl32(pb); // bucket-datasize
    avio_rl32(pb); // init-bucket-fullness
    avio_rl32(pb); // alt-leak-datarate
    avio_rl32(pb); // alt-bucket-datasize
    avio_rl32(pb); // alt-init-bucket-fullness
    avio_rl32(pb); // max-object-size
    avio_rl32(pb); // flags (reliable,seekable,no_cleanpoints?,resend-live-cleanpoints, rest of bits reserved)
    stream_num = avio_rl16(pb); // stream-num

    stream_languageid_index = avio_rl16(pb); // stream-language-id-index
    if (stream_num < 128)
        asf->streams[stream_num].stream_language_index = stream_languageid_index;

    avio_rl64(pb); // avg frametime in 100ns units
    stream_ct      = avio_rl16(pb); // stream-name-count
    payload_ext_ct = avio_rl16(pb); // payload-extension-system-count

    if (stream_num < 128) {
        asf->stream_bitrates[stream_num] = leak_rate;
        asf->streams[stream_num].payload_ext_ct = 0;
    }

    for (i = 0; i < stream_ct; i++) {
        avio_rl16(pb);
        ext_len = avio_rl16(pb);
        avio_skip(pb, ext_len);
    }

    for (i = 0; i < payload_ext_ct; i++) {
        int size;
        ff_get_guid(pb, &g);
        size = avio_rl16(pb);
        ext_len = avio_rl32(pb);
        avio_skip(pb, ext_len);
        if (stream_num < 128 && i < FF_ARRAY_ELEMS(asf->streams[stream_num].payload)) {
            ASFPayload *p = &asf->streams[stream_num].payload[i];
            p->type = g[0];
            p->size = size;
            av_log(s, AV_LOG_DEBUG, "Payload extension %x %d\n", g[0], p->size );
            asf->streams[stream_num].payload_ext_ct ++;
        }
    }

    return 0;
}
