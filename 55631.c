static int mxf_parse_package_comments(MXFContext *mxf, AVDictionary **pm, MXFPackage *package)
{
    MXFTaggedValue *tag;
    int size, i;
    char *key = NULL;

    for (i = 0; i < package->comment_count; i++) {
        tag = mxf_resolve_strong_ref(mxf, &package->comment_refs[i], TaggedValue);
        if (!tag || !tag->name || !tag->value)
            continue;

        size = strlen(tag->name) + 8 + 1;
        key = av_mallocz(size);
        if (!key)
            return AVERROR(ENOMEM);

        snprintf(key, size, "comment_%s", tag->name);
        av_dict_set(pm, key, tag->value, AV_DICT_DONT_STRDUP_KEY);
    }
    return 0;
}
