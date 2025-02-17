static int nsv_parse_NSVf_header(AVFormatContext *s)
{
    NSVContext *nsv = s->priv_data;
    AVIOContext *pb = s->pb;
    unsigned int av_unused file_size;
    unsigned int size;
    int64_t duration;
    int strings_size;
    int table_entries;
    int table_entries_used;

    nsv->state = NSV_UNSYNC; /* in case we fail */

    size = avio_rl32(pb);
    if (size < 28)
        return -1;
    nsv->NSVf_end = size;

    file_size = (uint32_t)avio_rl32(pb);
    av_log(s, AV_LOG_TRACE, "NSV NSVf chunk_size %u\n", size);
    av_log(s, AV_LOG_TRACE, "NSV NSVf file_size %u\n", file_size);

    nsv->duration = duration = avio_rl32(pb); /* in ms */
    av_log(s, AV_LOG_TRACE, "NSV NSVf duration %"PRId64" ms\n", duration);

    strings_size = avio_rl32(pb);
    table_entries = avio_rl32(pb);
    table_entries_used = avio_rl32(pb);
    av_log(s, AV_LOG_TRACE, "NSV NSVf info-strings size: %d, table entries: %d, bis %d\n",
            strings_size, table_entries, table_entries_used);
    if (pb->eof_reached)
        return -1;

    av_log(s, AV_LOG_TRACE, "NSV got header; filepos %"PRId64"\n", avio_tell(pb));

    if (strings_size > 0) {
        char *strings; /* last byte will be '\0' to play safe with str*() */
        char *p, *endp;
        char *token, *value;
        char quote;

        p = strings = av_mallocz((size_t)strings_size + 1);
        if (!p)
            return AVERROR(ENOMEM);
        endp = strings + strings_size;
        avio_read(pb, strings, strings_size);
        while (p < endp) {
            while (*p == ' ')
                p++; /* strip out spaces */
            if (p >= endp-2)
                break;
            token = p;
            p = strchr(p, '=');
            if (!p || p >= endp-2)
                break;
            *p++ = '\0';
            quote = *p++;
            value = p;
            p = strchr(p, quote);
            if (!p || p >= endp)
                break;
            *p++ = '\0';
            av_log(s, AV_LOG_TRACE, "NSV NSVf INFO: %s='%s'\n", token, value);
            av_dict_set(&s->metadata, token, value, 0);
        }
        av_free(strings);
    }
    if (pb->eof_reached)
        return -1;

    av_log(s, AV_LOG_TRACE, "NSV got infos; filepos %"PRId64"\n", avio_tell(pb));

    if (table_entries_used > 0) {
        int i;
        nsv->index_entries = table_entries_used;
        if((unsigned)table_entries_used >= UINT_MAX / sizeof(uint32_t))
            return -1;
        nsv->nsvs_file_offset = av_malloc((unsigned)table_entries_used * sizeof(uint32_t));
        if (!nsv->nsvs_file_offset)
            return AVERROR(ENOMEM);

        for(i=0;i<table_entries_used;i++)
            nsv->nsvs_file_offset[i] = avio_rl32(pb) + size;

        if(table_entries > table_entries_used &&
           avio_rl32(pb) == MKTAG('T','O','C','2')) {
            nsv->nsvs_timestamps = av_malloc((unsigned)table_entries_used*sizeof(uint32_t));
            if (!nsv->nsvs_timestamps)
                return AVERROR(ENOMEM);
            for(i=0;i<table_entries_used;i++) {
                nsv->nsvs_timestamps[i] = avio_rl32(pb);
            }
        }
    }

    av_log(s, AV_LOG_TRACE, "NSV got index; filepos %"PRId64"\n", avio_tell(pb));

    avio_seek(pb, nsv->base_offset + size, SEEK_SET); /* required for dumbdriving-271.nsv (2 extra bytes) */

    if (pb->eof_reached)
        return -1;
    nsv->state = NSV_HAS_READ_NSVF;
    return 0;
}
