static void var_read_metadata(AVFormatContext *avctx, const char *tag, int size)
{
    char *value = var_read_string(avctx->pb, size);
    if (value)
        av_dict_set(&avctx->metadata, tag, value, AV_DICT_DONT_STRDUP_VAL);
}
