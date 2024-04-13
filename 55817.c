static int save_avio_options(AVFormatContext *s)
{
    HLSContext *c = s->priv_data;
    static const char *opts[] = {
        "headers", "http_proxy", "user_agent", "user-agent", "cookies", NULL };
    const char **opt = opts;
    uint8_t *buf;
    int ret = 0;

    while (*opt) {
        if (av_opt_get(s->pb, *opt, AV_OPT_SEARCH_CHILDREN | AV_OPT_ALLOW_NULL, &buf) >= 0) {
            ret = av_dict_set(&c->avio_opts, *opt, buf,
                              AV_DICT_DONT_STRDUP_VAL);
            if (ret < 0)
                return ret;
        }
        opt++;
    }

    return ret;
}
