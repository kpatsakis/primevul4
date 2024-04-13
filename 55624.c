static int mxf_is_intra_only(MXFDescriptor *descriptor)
{
    return mxf_get_codec_ul(mxf_intra_only_essence_container_uls,
                            &descriptor->essence_container_ul)->id != AV_CODEC_ID_NONE ||
           mxf_get_codec_ul(mxf_intra_only_picture_essence_coding_uls,
                            &descriptor->essence_codec_ul)->id     != AV_CODEC_ID_NONE;
}
