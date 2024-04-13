static int mxf_add_metadata_stream(MXFContext *mxf, MXFTrack *track)
{
    MXFStructuralComponent *component = NULL;
    const MXFCodecUL *codec_ul = NULL;
    MXFPackage tmp_package;
    AVStream *st;
    int j;

    for (j = 0; j < track->sequence->structural_components_count; j++) {
        component = mxf_resolve_sourceclip(mxf, &track->sequence->structural_components_refs[j]);
        if (!component)
            continue;
        break;
    }
    if (!component)
        return 0;

    st = avformat_new_stream(mxf->fc, NULL);
    if (!st) {
        av_log(mxf->fc, AV_LOG_ERROR, "could not allocate metadata stream\n");
        return AVERROR(ENOMEM);
    }

    st->codecpar->codec_type = AVMEDIA_TYPE_DATA;
    st->codecpar->codec_id = AV_CODEC_ID_NONE;
    st->id = track->track_id;

    memcpy(&tmp_package.package_ul, component->source_package_ul, 16);
    memcpy(&tmp_package.package_uid, component->source_package_uid, 16);
    mxf_add_umid_metadata(&st->metadata, "file_package_umid", &tmp_package);
    if (track->name && track->name[0])
        av_dict_set(&st->metadata, "track_name", track->name, 0);

    codec_ul = mxf_get_codec_ul(ff_mxf_data_definition_uls, &track->sequence->data_definition_ul);
    av_dict_set(&st->metadata, "data_type", av_get_media_type_string(codec_ul->id), 0);
    return 0;
}
