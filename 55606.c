static int mxf_add_metadata_set(MXFContext *mxf, void *metadata_set)
{
    MXFMetadataSet **tmp;

    tmp = av_realloc_array(mxf->metadata_sets, mxf->metadata_sets_count + 1, sizeof(*mxf->metadata_sets));
    if (!tmp)
        return AVERROR(ENOMEM);
    mxf->metadata_sets = tmp;
    mxf->metadata_sets[mxf->metadata_sets_count] = metadata_set;
    mxf->metadata_sets_count++;
    return 0;
}
