int ff_rm_read_mdpr_codecdata(AVFormatContext *s, AVIOContext *pb,
                              AVStream *st, RMStream *rst,
                              unsigned int codec_data_size, const uint8_t *mime)
{
    unsigned int v;
    int size;
    int64_t codec_pos;
    int ret;

    if (codec_data_size > INT_MAX)
        return AVERROR_INVALIDDATA;
    if (codec_data_size == 0)
        return 0;

    avpriv_set_pts_info(st, 64, 1, 1000);
    codec_pos = avio_tell(pb);
    v = avio_rb32(pb);

    if (v == MKTAG(0xfd, 'a', 'r', '.')) {
        /* ra type header */
        if (rm_read_audio_stream_info(s, pb, st, rst, 0))
            return -1;
    } else if (v == MKBETAG('L', 'S', 'D', ':')) {
        avio_seek(pb, -4, SEEK_CUR);
        if ((ret = rm_read_extradata(s, pb, st->codecpar, codec_data_size)) < 0)
            return ret;

        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_tag  = AV_RL32(st->codecpar->extradata);
        st->codecpar->codec_id   = ff_codec_get_id(ff_rm_codec_tags,
                                                st->codecpar->codec_tag);
    } else if(mime && !strcmp(mime, "logical-fileinfo")){
        int stream_count, rule_count, property_count, i;
        ff_free_stream(s, st);
        if (avio_rb16(pb) != 0) {
            av_log(s, AV_LOG_WARNING, "Unsupported version\n");
            goto skip;
        }
        stream_count = avio_rb16(pb);
        avio_skip(pb, 6*stream_count);
        rule_count = avio_rb16(pb);
        avio_skip(pb, 2*rule_count);
        property_count = avio_rb16(pb);
        for(i=0; i<property_count; i++){
            uint8_t name[128], val[128];
            avio_rb32(pb);
            if (avio_rb16(pb) != 0) {
                av_log(s, AV_LOG_WARNING, "Unsupported Name value property version\n");
                goto skip; //FIXME skip just this one
            }
            get_str8(pb, name, sizeof(name));
            switch(avio_rb32(pb)) {
            case 2: get_strl(pb, val, sizeof(val), avio_rb16(pb));
                av_dict_set(&s->metadata, name, val, 0);
                break;
            default: avio_skip(pb, avio_rb16(pb));
            }
        }
    } else {
        int fps;
        if (avio_rl32(pb) != MKTAG('V', 'I', 'D', 'O')) {
        fail1:
            av_log(s, AV_LOG_WARNING, "Unsupported stream type %08x\n", v);
            goto skip;
        }
        st->codecpar->codec_tag = avio_rl32(pb);
        st->codecpar->codec_id  = ff_codec_get_id(ff_rm_codec_tags,
                                                  st->codecpar->codec_tag);
        av_log(s, AV_LOG_TRACE, "%"PRIX32" %X\n",
               st->codecpar->codec_tag, MKTAG('R', 'V', '2', '0'));
        if (st->codecpar->codec_id == AV_CODEC_ID_NONE)
            goto fail1;
        st->codecpar->width  = avio_rb16(pb);
        st->codecpar->height = avio_rb16(pb);
        avio_skip(pb, 2); // looks like bits per sample
        avio_skip(pb, 4); // always zero?
        st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        st->need_parsing = AVSTREAM_PARSE_TIMESTAMPS;
        fps = avio_rb32(pb);

        if ((ret = rm_read_extradata(s, pb, st->codecpar, codec_data_size - (avio_tell(pb) - codec_pos))) < 0)
            return ret;

        if (fps > 0) {
            av_reduce(&st->avg_frame_rate.den, &st->avg_frame_rate.num,
                      0x10000, fps, (1 << 30) - 1);
#if FF_API_R_FRAME_RATE
            st->r_frame_rate = st->avg_frame_rate;
#endif
        } else if (s->error_recognition & AV_EF_EXPLODE) {
            av_log(s, AV_LOG_ERROR, "Invalid framerate\n");
            return AVERROR_INVALIDDATA;
        }
    }

skip:
    /* skip codec info */
    size = avio_tell(pb) - codec_pos;
    if (codec_data_size >= size) {
        avio_skip(pb, codec_data_size - size);
    } else {
        av_log(s, AV_LOG_WARNING, "codec_data_size %u < size %d\n", codec_data_size, size);
    }

    return 0;
}
