int attribute_align_arg avcodec_decode_audio4(AVCodecContext *avctx,
                                              AVFrame *frame,
                                              int *got_frame_ptr,
                                              const AVPacket *avpkt)
{
    AVCodecInternal *avci = avctx->internal;
    int ret = 0;

    *got_frame_ptr = 0;

    if (!avctx->codec)
        return AVERROR(EINVAL);

    if (!avctx->codec->decode) {
        av_log(avctx, AV_LOG_ERROR, "This decoder requires using the avcodec_send_packet() API.\n");
        return AVERROR(ENOSYS);
    }

    if (!avpkt->data && avpkt->size) {
        av_log(avctx, AV_LOG_ERROR, "invalid packet: NULL data, size != 0\n");
        return AVERROR(EINVAL);
    }
    if (avctx->codec->type != AVMEDIA_TYPE_AUDIO) {
        av_log(avctx, AV_LOG_ERROR, "Invalid media type for audio\n");
        return AVERROR(EINVAL);
    }

    av_frame_unref(frame);

    if ((avctx->codec->capabilities & AV_CODEC_CAP_DELAY) || avpkt->size || (avctx->active_thread_type & FF_THREAD_FRAME)) {
        uint8_t *side;
        int side_size;
        uint32_t discard_padding = 0;
        uint8_t skip_reason = 0;
        uint8_t discard_reason = 0;
        AVPacket tmp = *avpkt;
        int did_split = av_packet_split_side_data(&tmp);
        ret = apply_param_change(avctx, &tmp);
        if (ret < 0)
            goto fail;

        avctx->internal->pkt = &tmp;
        if (HAVE_THREADS && avctx->active_thread_type & FF_THREAD_FRAME)
            ret = ff_thread_decode_frame(avctx, frame, got_frame_ptr, &tmp);
        else {
            ret = avctx->codec->decode(avctx, frame, got_frame_ptr, &tmp);
            av_assert0(ret <= tmp.size);
            frame->pkt_dts = avpkt->dts;
        }
        if (ret >= 0 && *got_frame_ptr) {
            avctx->frame_number++;
            av_frame_set_best_effort_timestamp(frame,
                                               guess_correct_pts(avctx,
                                                                 frame->pts,
                                                                 frame->pkt_dts));
            if (frame->format == AV_SAMPLE_FMT_NONE)
                frame->format = avctx->sample_fmt;
            if (!frame->channel_layout)
                frame->channel_layout = avctx->channel_layout;
            if (!av_frame_get_channels(frame))
                av_frame_set_channels(frame, avctx->channels);
            if (!frame->sample_rate)
                frame->sample_rate = avctx->sample_rate;
        }

        side= av_packet_get_side_data(avctx->internal->pkt, AV_PKT_DATA_SKIP_SAMPLES, &side_size);
        if(side && side_size>=10) {
            avctx->internal->skip_samples = AV_RL32(side);
            discard_padding = AV_RL32(side + 4);
            av_log(avctx, AV_LOG_DEBUG, "skip %d / discard %d samples due to side data\n",
                   avctx->internal->skip_samples, (int)discard_padding);
            skip_reason = AV_RL8(side + 8);
            discard_reason = AV_RL8(side + 9);
        }

        if ((frame->flags & AV_FRAME_FLAG_DISCARD) && *got_frame_ptr &&
            !(avctx->flags2 & AV_CODEC_FLAG2_SKIP_MANUAL)) {
            avctx->internal->skip_samples = FFMAX(0, avctx->internal->skip_samples - frame->nb_samples);
            *got_frame_ptr = 0;
        }

        if (avctx->internal->skip_samples > 0 && *got_frame_ptr &&
            !(avctx->flags2 & AV_CODEC_FLAG2_SKIP_MANUAL)) {
            if(frame->nb_samples <= avctx->internal->skip_samples){
                *got_frame_ptr = 0;
                avctx->internal->skip_samples -= frame->nb_samples;
                av_log(avctx, AV_LOG_DEBUG, "skip whole frame, skip left: %d\n",
                       avctx->internal->skip_samples);
            } else {
                av_samples_copy(frame->extended_data, frame->extended_data, 0, avctx->internal->skip_samples,
                                frame->nb_samples - avctx->internal->skip_samples, avctx->channels, frame->format);
                if(avctx->pkt_timebase.num && avctx->sample_rate) {
                    int64_t diff_ts = av_rescale_q(avctx->internal->skip_samples,
                                                   (AVRational){1, avctx->sample_rate},
                                                   avctx->pkt_timebase);
                    if(frame->pts!=AV_NOPTS_VALUE)
                        frame->pts += diff_ts;
#if FF_API_PKT_PTS
FF_DISABLE_DEPRECATION_WARNINGS
                    if(frame->pkt_pts!=AV_NOPTS_VALUE)
                        frame->pkt_pts += diff_ts;
FF_ENABLE_DEPRECATION_WARNINGS
#endif
                    if(frame->pkt_dts!=AV_NOPTS_VALUE)
                        frame->pkt_dts += diff_ts;
                    if (av_frame_get_pkt_duration(frame) >= diff_ts)
                        av_frame_set_pkt_duration(frame, av_frame_get_pkt_duration(frame) - diff_ts);
                } else {
                    av_log(avctx, AV_LOG_WARNING, "Could not update timestamps for skipped samples.\n");
                }
                av_log(avctx, AV_LOG_DEBUG, "skip %d/%d samples\n",
                       avctx->internal->skip_samples, frame->nb_samples);
                frame->nb_samples -= avctx->internal->skip_samples;
                avctx->internal->skip_samples = 0;
            }
        }

        if (discard_padding > 0 && discard_padding <= frame->nb_samples && *got_frame_ptr &&
            !(avctx->flags2 & AV_CODEC_FLAG2_SKIP_MANUAL)) {
            if (discard_padding == frame->nb_samples) {
                *got_frame_ptr = 0;
            } else {
                if(avctx->pkt_timebase.num && avctx->sample_rate) {
                    int64_t diff_ts = av_rescale_q(frame->nb_samples - discard_padding,
                                                   (AVRational){1, avctx->sample_rate},
                                                   avctx->pkt_timebase);
                    av_frame_set_pkt_duration(frame, diff_ts);
                } else {
                    av_log(avctx, AV_LOG_WARNING, "Could not update timestamps for discarded samples.\n");
                }
                av_log(avctx, AV_LOG_DEBUG, "discard %d/%d samples\n",
                       (int)discard_padding, frame->nb_samples);
                frame->nb_samples -= discard_padding;
            }
        }

        if ((avctx->flags2 & AV_CODEC_FLAG2_SKIP_MANUAL) && *got_frame_ptr) {
            AVFrameSideData *fside = av_frame_new_side_data(frame, AV_FRAME_DATA_SKIP_SAMPLES, 10);
            if (fside) {
                AV_WL32(fside->data, avctx->internal->skip_samples);
                AV_WL32(fside->data + 4, discard_padding);
                AV_WL8(fside->data + 8, skip_reason);
                AV_WL8(fside->data + 9, discard_reason);
                avctx->internal->skip_samples = 0;
            }
        }
fail:
        avctx->internal->pkt = NULL;
        if (did_split) {
            av_packet_free_side_data(&tmp);
            if(ret == tmp.size)
                ret = avpkt->size;
        }

        if (ret >= 0 && *got_frame_ptr) {
            if (!avctx->refcounted_frames) {
                int err = unrefcount_frame(avci, frame);
                if (err < 0)
                    return err;
            }
        } else
            av_frame_unref(frame);
    }

    av_assert0(ret <= avpkt->size);

    if (!avci->showed_multi_packet_warning &&
        ret >= 0 && ret != avpkt->size && !(avctx->codec->capabilities & AV_CODEC_CAP_SUBFRAMES)) {
            av_log(avctx, AV_LOG_WARNING, "Multiple frames in a packet.\n");
        avci->showed_multi_packet_warning = 1;
    }

    return ret;
}
