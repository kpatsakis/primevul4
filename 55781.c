static int set_metadata_float(AVDictionary **dict, const char *key, float value, int allow_zero)
{
    if (value != 0 || allow_zero) {
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "%f", value);
        return av_dict_set(dict, key, tmp, 0);
    }
    return 0;
}
