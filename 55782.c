static int set_metadata_int(AVDictionary **dict, const char *key, int value, int allow_zero)
{
    if (value || allow_zero) {
        return av_dict_set_int(dict, key, value, 0);
    }
    return 0;
}
