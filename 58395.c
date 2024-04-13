static void codec_parameters_reset(AVCodecParameters *par)
{
    av_freep(&par->extradata);

    memset(par, 0, sizeof(*par));

    par->codec_type          = AVMEDIA_TYPE_UNKNOWN;
    par->codec_id            = AV_CODEC_ID_NONE;
    par->format              = -1;
    par->field_order         = AV_FIELD_UNKNOWN;
    par->color_range         = AVCOL_RANGE_UNSPECIFIED;
    par->color_primaries     = AVCOL_PRI_UNSPECIFIED;
    par->color_trc           = AVCOL_TRC_UNSPECIFIED;
    par->color_space         = AVCOL_SPC_UNSPECIFIED;
    par->chroma_location     = AVCHROMA_LOC_UNSPECIFIED;
    par->sample_aspect_ratio = (AVRational){ 0, 1 };
    par->profile             = FF_PROFILE_UNKNOWN;
    par->level               = FF_LEVEL_UNKNOWN;
}
