static int mov_skip_multiple_stsd(MOVContext *c, AVIOContext *pb,
                                  int codec_tag, int format,
                                  int64_t size)
{
    int video_codec_id = ff_codec_get_id(ff_codec_movvideo_tags, format);

    if (codec_tag &&
         (codec_tag != format &&
          (codec_tag != AV_RL32("AV1x") || format != AV_RL32("AVup")) &&
          codec_tag != AV_RL32("apcn") && codec_tag != AV_RL32("apch") &&
          codec_tag != AV_RL32("dvpp") && codec_tag != AV_RL32("dvcp") &&
          (c->fc->video_codec_id ? video_codec_id != c->fc->video_codec_id
                                 : codec_tag != MKTAG('j','p','e','g')))) {
        /* Multiple fourcc, we skip JPEG. This is not correct, we should
         * export it as a separate AVStream but this needs a few changes
         * in the MOV demuxer, patch welcome. */

        av_log(c->fc, AV_LOG_WARNING, "multiple fourcc not supported\n");
        avio_skip(pb, size);
        return 1;
    }

    return 0;
}
