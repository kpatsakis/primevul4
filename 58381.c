const char *avcodec_profile_name(enum AVCodecID codec_id, int profile)
{
    const AVCodecDescriptor *desc = avcodec_descriptor_get(codec_id);
    const AVProfile *p;

    if (profile == FF_PROFILE_UNKNOWN || !desc || !desc->profiles)
        return NULL;

    for (p = desc->profiles; p->profile != FF_PROFILE_UNKNOWN; p++)
        if (p->profile == profile)
            return p->name;

    return NULL;
}
