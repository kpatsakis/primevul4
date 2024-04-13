static int parse_video_var(AVFormatContext *avctx, AVStream *st,
                           const char *name, int size)
{
    AVIOContext *pb = avctx->pb;
    if (!strcmp(name, "__DIR_COUNT")) {
        st->nb_frames = st->duration = var_read_int(pb, size);
    } else if (!strcmp(name, "COMPRESSION")) {
        char *str = var_read_string(pb, size);
        if (!str)
            return AVERROR_INVALIDDATA;
        if (!strcmp(str, "1")) {
            st->codecpar->codec_id = AV_CODEC_ID_MVC1;
        } else if (!strcmp(str, "2")) {
            st->codecpar->format = AV_PIX_FMT_ABGR;
            st->codecpar->codec_id = AV_CODEC_ID_RAWVIDEO;
        } else if (!strcmp(str, "3")) {
            st->codecpar->codec_id = AV_CODEC_ID_SGIRLE;
        } else if (!strcmp(str, "10")) {
            st->codecpar->codec_id = AV_CODEC_ID_MJPEG;
        } else if (!strcmp(str, "MVC2")) {
            st->codecpar->codec_id = AV_CODEC_ID_MVC2;
        } else {
            avpriv_request_sample(avctx, "Video compression %s", str);
        }
        av_free(str);
    } else if (!strcmp(name, "FPS")) {
        AVRational fps = var_read_float(pb, size);
        avpriv_set_pts_info(st, 64, fps.den, fps.num);
        st->avg_frame_rate = fps;
    } else if (!strcmp(name, "HEIGHT")) {
        st->codecpar->height = var_read_int(pb, size);
    } else if (!strcmp(name, "PIXEL_ASPECT")) {
        st->sample_aspect_ratio = var_read_float(pb, size);
        av_reduce(&st->sample_aspect_ratio.num, &st->sample_aspect_ratio.den,
                  st->sample_aspect_ratio.num, st->sample_aspect_ratio.den,
                  INT_MAX);
    } else if (!strcmp(name, "WIDTH")) {
        st->codecpar->width = var_read_int(pb, size);
    } else if (!strcmp(name, "ORIENTATION")) {
        if (var_read_int(pb, size) == 1101) {
            st->codecpar->extradata      = av_strdup("BottomUp");
            st->codecpar->extradata_size = 9;
        }
    } else if (!strcmp(name, "Q_SPATIAL") || !strcmp(name, "Q_TEMPORAL")) {
        var_read_metadata(avctx, name, size);
    } else if (!strcmp(name, "INTERLACING") || !strcmp(name, "PACKING")) {
        avio_skip(pb, size); // ignore
    } else
        return AVERROR_INVALIDDATA;

    return 0;
}
