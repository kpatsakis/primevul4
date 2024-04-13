static void *mxf_resolve_strong_ref(MXFContext *mxf, UID *strong_ref, enum MXFMetadataSetType type)
{
    int i;

    if (!strong_ref)
        return NULL;
    for (i = 0; i < mxf->metadata_sets_count; i++) {
        if (!memcmp(*strong_ref, mxf->metadata_sets[i]->uid, 16) &&
            (type == AnyType || mxf->metadata_sets[i]->type == type)) {
            return mxf->metadata_sets[i];
        }
    }
    return NULL;
}
