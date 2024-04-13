int avpriv_codec_get_cap_skip_frame_fill_param(const AVCodec *codec){
    return !!(codec->caps_internal & FF_CODEC_CAP_SKIP_FRAME_FILL_PARAM);
}
