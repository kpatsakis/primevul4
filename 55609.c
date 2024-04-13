static int mxf_add_umid_metadata(AVDictionary **pm, const char *key, MXFPackage* package)
{
    char *str;
    int ret;
    if (!package)
        return 0;
    if ((ret = mxf_umid_to_str(package->package_ul, package->package_uid, &str)) < 0)
        return ret;
    av_dict_set(pm, key, str, AV_DICT_DONT_STRDUP_VAL);
    return 0;
}
