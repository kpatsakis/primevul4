static int mxf_parse_physical_source_package(MXFContext *mxf, MXFTrack *source_track, AVStream *st)
{
    MXFPackage *physical_package = NULL;
    MXFTrack *physical_track = NULL;
    MXFStructuralComponent *sourceclip = NULL;
    MXFTimecodeComponent *mxf_tc = NULL;
    int i, j, k;
    AVTimecode tc;
    int flags;
    int64_t start_position;

    for (i = 0; i < source_track->sequence->structural_components_count; i++) {
        sourceclip = mxf_resolve_strong_ref(mxf, &source_track->sequence->structural_components_refs[i], SourceClip);
        if (!sourceclip)
            continue;

        if (!(physical_package = mxf_resolve_source_package(mxf, sourceclip->source_package_uid)))
            break;

        mxf_add_umid_metadata(&st->metadata, "reel_umid", physical_package);

        /* the name of physical source package is name of the reel or tape */
        if (physical_package->name && physical_package->name[0])
            av_dict_set(&st->metadata, "reel_name", physical_package->name, 0);

        /* the source timecode is calculated by adding the start_position of the sourceclip from the file source package track
         * to the start_frame of the timecode component located on one of the tracks of the physical source package.
         */
        for (j = 0; j < physical_package->tracks_count; j++) {
            if (!(physical_track = mxf_resolve_strong_ref(mxf, &physical_package->tracks_refs[j], Track))) {
                av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track strong ref\n");
                continue;
            }

            if (!(physical_track->sequence = mxf_resolve_strong_ref(mxf, &physical_track->sequence_ref, Sequence))) {
                av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track sequence strong ref\n");
                continue;
            }

        if (physical_track->edit_rate.num <= 0 ||
            physical_track->edit_rate.den <= 0) {
            av_log(mxf->fc, AV_LOG_WARNING,
                   "Invalid edit rate (%d/%d) found on structural"
                   " component #%d, defaulting to 25/1\n",
                   physical_track->edit_rate.num,
                   physical_track->edit_rate.den, i);
            physical_track->edit_rate = (AVRational){25, 1};
        }

            for (k = 0; k < physical_track->sequence->structural_components_count; k++) {
                if (!(mxf_tc = mxf_resolve_timecode_component(mxf, &physical_track->sequence->structural_components_refs[k])))
                    continue;

                flags = mxf_tc->drop_frame == 1 ? AV_TIMECODE_FLAG_DROPFRAME : 0;
                /* scale sourceclip start_position to match physical track edit rate */
                start_position = av_rescale_q(sourceclip->start_position,
                                              physical_track->edit_rate,
                                              source_track->edit_rate);

                if (av_timecode_init(&tc, mxf_tc->rate, flags, start_position + mxf_tc->start_frame, mxf->fc) == 0) {
                    mxf_add_timecode_metadata(&st->metadata, "timecode", &tc);
                    return 0;
                }
            }
        }
    }

    return 0;
}
