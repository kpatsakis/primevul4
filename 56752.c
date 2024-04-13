static av_cold int ape_decode_init(AVCodecContext *avctx)
{
    APEContext *s = avctx->priv_data;
    int i;

    if (avctx->extradata_size != 6) {
        av_log(avctx, AV_LOG_ERROR, "Incorrect extradata\n");
        return AVERROR(EINVAL);
    }
    if (avctx->channels > 2) {
        av_log(avctx, AV_LOG_ERROR, "Only mono and stereo is supported\n");
        return AVERROR(EINVAL);
    }
    s->bps = avctx->bits_per_coded_sample;
    switch (s->bps) {
    case 8:
        avctx->sample_fmt = AV_SAMPLE_FMT_U8P;
        break;
    case 16:
        avctx->sample_fmt = AV_SAMPLE_FMT_S16P;
        break;
    case 24:
        avctx->sample_fmt = AV_SAMPLE_FMT_S32P;
        break;
    default:
        avpriv_request_sample(avctx,
                              "%d bits per coded sample", s->bps);
        return AVERROR_PATCHWELCOME;
    }
    s->avctx             = avctx;
    s->channels          = avctx->channels;
    s->fileversion       = AV_RL16(avctx->extradata);
    s->compression_level = AV_RL16(avctx->extradata + 2);
    s->flags             = AV_RL16(avctx->extradata + 4);

    av_log(avctx, AV_LOG_VERBOSE, "Compression Level: %d - Flags: %d\n",
           s->compression_level, s->flags);
    if (s->compression_level % 1000 || s->compression_level > COMPRESSION_LEVEL_INSANE ||
        !s->compression_level ||
        (s->fileversion < 3930 && s->compression_level == COMPRESSION_LEVEL_INSANE)) {
        av_log(avctx, AV_LOG_ERROR, "Incorrect compression level %d\n",
               s->compression_level);
        return AVERROR_INVALIDDATA;
    }
    s->fset = s->compression_level / 1000 - 1;
    for (i = 0; i < APE_FILTER_LEVELS; i++) {
        if (!ape_filter_orders[s->fset][i])
            break;
        FF_ALLOC_OR_GOTO(avctx, s->filterbuf[i],
                         (ape_filter_orders[s->fset][i] * 3 + HISTORY_SIZE) * 4,
                         filter_alloc_fail);
    }

    if (s->fileversion < 3860) {
        s->entropy_decode_mono   = entropy_decode_mono_0000;
        s->entropy_decode_stereo = entropy_decode_stereo_0000;
    } else if (s->fileversion < 3900) {
        s->entropy_decode_mono   = entropy_decode_mono_3860;
        s->entropy_decode_stereo = entropy_decode_stereo_3860;
    } else if (s->fileversion < 3930) {
        s->entropy_decode_mono   = entropy_decode_mono_3900;
        s->entropy_decode_stereo = entropy_decode_stereo_3900;
    } else if (s->fileversion < 3990) {
        s->entropy_decode_mono   = entropy_decode_mono_3900;
        s->entropy_decode_stereo = entropy_decode_stereo_3930;
    } else {
        s->entropy_decode_mono   = entropy_decode_mono_3990;
        s->entropy_decode_stereo = entropy_decode_stereo_3990;
    }

    if (s->fileversion < 3930) {
        s->predictor_decode_mono   = predictor_decode_mono_3800;
        s->predictor_decode_stereo = predictor_decode_stereo_3800;
    } else if (s->fileversion < 3950) {
        s->predictor_decode_mono   = predictor_decode_mono_3930;
        s->predictor_decode_stereo = predictor_decode_stereo_3930;
    } else {
        s->predictor_decode_mono   = predictor_decode_mono_3950;
        s->predictor_decode_stereo = predictor_decode_stereo_3950;
    }

    ff_bswapdsp_init(&s->bdsp);
    ff_llauddsp_init(&s->adsp);
    avctx->channel_layout = (avctx->channels==2) ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO;

    return 0;
filter_alloc_fail:
    ape_decode_close(avctx);
    return AVERROR(ENOMEM);
}
