int ff_init_buffer_info(AVCodecContext *avctx, AVFrame *frame)
{
    AVPacket *pkt = avctx->internal->pkt;
    int i;
    static const struct {
        enum AVPacketSideDataType packet;
        enum AVFrameSideDataType frame;
    } sd[] = {
        { AV_PKT_DATA_REPLAYGAIN ,                AV_FRAME_DATA_REPLAYGAIN },
        { AV_PKT_DATA_DISPLAYMATRIX,              AV_FRAME_DATA_DISPLAYMATRIX },
        { AV_PKT_DATA_SPHERICAL,                  AV_FRAME_DATA_SPHERICAL },
        { AV_PKT_DATA_STEREO3D,                   AV_FRAME_DATA_STEREO3D },
        { AV_PKT_DATA_AUDIO_SERVICE_TYPE,         AV_FRAME_DATA_AUDIO_SERVICE_TYPE },
        { AV_PKT_DATA_MASTERING_DISPLAY_METADATA, AV_FRAME_DATA_MASTERING_DISPLAY_METADATA },
    };

    if (pkt) {
        frame->pts = pkt->pts;
#if FF_API_PKT_PTS
FF_DISABLE_DEPRECATION_WARNINGS
        frame->pkt_pts = pkt->pts;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
        av_frame_set_pkt_pos     (frame, pkt->pos);
        av_frame_set_pkt_duration(frame, pkt->duration);
        av_frame_set_pkt_size    (frame, pkt->size);

        for (i = 0; i < FF_ARRAY_ELEMS(sd); i++) {
            int size;
            uint8_t *packet_sd = av_packet_get_side_data(pkt, sd[i].packet, &size);
            if (packet_sd) {
                AVFrameSideData *frame_sd = av_frame_new_side_data(frame,
                                                                   sd[i].frame,
                                                                   size);
                if (!frame_sd)
                    return AVERROR(ENOMEM);

                memcpy(frame_sd->data, packet_sd, size);
            }
        }
        add_metadata_from_side_data(pkt, frame);

        if (pkt->flags & AV_PKT_FLAG_DISCARD) {
            frame->flags |= AV_FRAME_FLAG_DISCARD;
        } else {
            frame->flags = (frame->flags & ~AV_FRAME_FLAG_DISCARD);
        }
    } else {
        frame->pts = AV_NOPTS_VALUE;
#if FF_API_PKT_PTS
FF_DISABLE_DEPRECATION_WARNINGS
        frame->pkt_pts = AV_NOPTS_VALUE;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
        av_frame_set_pkt_pos     (frame, -1);
        av_frame_set_pkt_duration(frame, 0);
        av_frame_set_pkt_size    (frame, -1);
    }
    frame->reordered_opaque = avctx->reordered_opaque;

    if (frame->color_primaries == AVCOL_PRI_UNSPECIFIED)
        frame->color_primaries = avctx->color_primaries;
    if (frame->color_trc == AVCOL_TRC_UNSPECIFIED)
        frame->color_trc = avctx->color_trc;
    if (av_frame_get_colorspace(frame) == AVCOL_SPC_UNSPECIFIED)
        av_frame_set_colorspace(frame, avctx->colorspace);
    if (av_frame_get_color_range(frame) == AVCOL_RANGE_UNSPECIFIED)
        av_frame_set_color_range(frame, avctx->color_range);
    if (frame->chroma_location == AVCHROMA_LOC_UNSPECIFIED)
        frame->chroma_location = avctx->chroma_sample_location;

    switch (avctx->codec->type) {
    case AVMEDIA_TYPE_VIDEO:
        frame->format              = avctx->pix_fmt;
        if (!frame->sample_aspect_ratio.num)
            frame->sample_aspect_ratio = avctx->sample_aspect_ratio;

        if (frame->width && frame->height &&
            av_image_check_sar(frame->width, frame->height,
                               frame->sample_aspect_ratio) < 0) {
            av_log(avctx, AV_LOG_WARNING, "ignoring invalid SAR: %u/%u\n",
                   frame->sample_aspect_ratio.num,
                   frame->sample_aspect_ratio.den);
            frame->sample_aspect_ratio = (AVRational){ 0, 1 };
        }

        break;
    case AVMEDIA_TYPE_AUDIO:
        if (!frame->sample_rate)
            frame->sample_rate    = avctx->sample_rate;
        if (frame->format < 0)
            frame->format         = avctx->sample_fmt;
        if (!frame->channel_layout) {
            if (avctx->channel_layout) {
                 if (av_get_channel_layout_nb_channels(avctx->channel_layout) !=
                     avctx->channels) {
                     av_log(avctx, AV_LOG_ERROR, "Inconsistent channel "
                            "configuration.\n");
                     return AVERROR(EINVAL);
                 }

                frame->channel_layout = avctx->channel_layout;
            } else {
                if (avctx->channels > FF_SANE_NB_CHANNELS) {
                    av_log(avctx, AV_LOG_ERROR, "Too many channels: %d.\n",
                           avctx->channels);
                    return AVERROR(ENOSYS);
                }
            }
        }
        av_frame_set_channels(frame, avctx->channels);
        break;
    }
    return 0;
}
