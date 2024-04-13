static int nsv_parse_NSVs_header(AVFormatContext *s)
{
    NSVContext *nsv = s->priv_data;
    AVIOContext *pb = s->pb;
    uint32_t vtag, atag;
    uint16_t vwidth, vheight;
    AVRational framerate;
    int i;
    AVStream *st;
    NSVStream *nst;

    vtag = avio_rl32(pb);
    atag = avio_rl32(pb);
    vwidth = avio_rl16(pb);
    vheight = avio_rl16(pb);
    i = avio_r8(pb);

    av_log(s, AV_LOG_TRACE, "NSV NSVs framerate code %2x\n", i);
    if(i&0x80) { /* odd way of giving native framerates from docs */
        int t=(i & 0x7F)>>2;
        if(t<16) framerate = (AVRational){1, t+1};
        else     framerate = (AVRational){t-15, 1};

        if(i&1){
            framerate.num *= 1000;
            framerate.den *= 1001;
        }

        if((i&3)==3)      framerate.num *= 24;
        else if((i&3)==2) framerate.num *= 25;
        else              framerate.num *= 30;
    }
    else
        framerate= (AVRational){i, 1};

    nsv->avsync = avio_rl16(pb);
    nsv->framerate = framerate;

    av_log(s, AV_LOG_TRACE, "NSV NSVs vsize %"PRIu16"x%"PRIu16"\n",
           vwidth, vheight);

    /* XXX change to ap != NULL ? */
    if (s->nb_streams == 0) { /* streams not yet published, let's do that */
        nsv->vtag = vtag;
        nsv->atag = atag;
        nsv->vwidth = vwidth;
        nsv->vheight = vwidth;
        if (vtag != T_NONE) {
            int i;
            st = avformat_new_stream(s, NULL);
            if (!st)
                goto fail;

            st->id = NSV_ST_VIDEO;
            nst = av_mallocz(sizeof(NSVStream));
            if (!nst)
                goto fail;
            st->priv_data = nst;
            st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
            st->codecpar->codec_tag = vtag;
            st->codecpar->codec_id = ff_codec_get_id(nsv_codec_video_tags, vtag);
            st->codecpar->width = vwidth;
            st->codecpar->height = vheight;
            st->codecpar->bits_per_coded_sample = 24; /* depth XXX */

            avpriv_set_pts_info(st, 64, framerate.den, framerate.num);
            st->start_time = 0;
            st->duration = av_rescale(nsv->duration, framerate.num, 1000*framerate.den);

            for(i=0;i<nsv->index_entries;i++) {
                if(nsv->nsvs_timestamps) {
                    av_add_index_entry(st, nsv->nsvs_file_offset[i], nsv->nsvs_timestamps[i],
                                       0, 0, AVINDEX_KEYFRAME);
                } else {
                    int64_t ts = av_rescale(i*nsv->duration/nsv->index_entries, framerate.num, 1000*framerate.den);
                    av_add_index_entry(st, nsv->nsvs_file_offset[i], ts, 0, 0, AVINDEX_KEYFRAME);
                }
            }
        }
        if (atag != T_NONE) {
            st = avformat_new_stream(s, NULL);
            if (!st)
                goto fail;

            st->id = NSV_ST_AUDIO;
            nst = av_mallocz(sizeof(NSVStream));
            if (!nst)
                goto fail;
            st->priv_data = nst;
            st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
            st->codecpar->codec_tag = atag;
            st->codecpar->codec_id = ff_codec_get_id(nsv_codec_audio_tags, atag);

            st->need_parsing = AVSTREAM_PARSE_FULL; /* for PCM we will read a chunk later and put correct info */

            /* set timebase to common denominator of ms and framerate */
            avpriv_set_pts_info(st, 64, 1, framerate.num*1000);
            st->start_time = 0;
            st->duration = (int64_t)nsv->duration * framerate.num;
        }
    } else {
        if (nsv->vtag != vtag || nsv->atag != atag || nsv->vwidth != vwidth || nsv->vheight != vwidth) {
            av_log(s, AV_LOG_TRACE, "NSV NSVs header values differ from the first one!!!\n");
        }
    }

    nsv->state = NSV_HAS_READ_NSVS;
    return 0;
fail:
    /* XXX */
    nsv->state = NSV_UNSYNC;
    return -1;
}
