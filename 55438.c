static void get_tag(AVFormatContext *s, const char *key, int type, int len, int type2_size)
{
    ASFContext *asf = s->priv_data;
    char *value = NULL;
    int64_t off = avio_tell(s->pb);
#define LEN 22

    if ((unsigned)len >= (UINT_MAX - LEN) / 2)
        return;

    if (!asf->export_xmp && !strncmp(key, "xmp", 3))
        goto finish;

    value = av_malloc(2 * len + LEN);
    if (!value)
        goto finish;

    switch (type) {
    case ASF_UNICODE:
        avio_get_str16le(s->pb, len, value, 2 * len + 1);
        break;
    case -1: // ASCI
        avio_read(s->pb, value, len);
        value[len]=0;
        break;
    case ASF_BYTE_ARRAY:
        if (!strcmp(key, "WM/Picture")) { // handle cover art
            asf_read_picture(s, len);
        } else if (!strcmp(key, "ID3")) { // handle ID3 tag
            get_id3_tag(s, len);
        } else {
            av_log(s, AV_LOG_VERBOSE, "Unsupported byte array in tag %s.\n", key);
        }
        goto finish;
    case ASF_BOOL:
    case ASF_DWORD:
    case ASF_QWORD:
    case ASF_WORD: {
        uint64_t num = get_value(s->pb, type, type2_size);
        snprintf(value, LEN, "%"PRIu64, num);
        break;
    }
    case ASF_GUID:
        av_log(s, AV_LOG_DEBUG, "Unsupported GUID value in tag %s.\n", key);
        goto finish;
    default:
        av_log(s, AV_LOG_DEBUG,
               "Unsupported value type %d in tag %s.\n", type, key);
        goto finish;
    }
    if (*value)
        av_dict_set(&s->metadata, key, value, 0);

finish:
    av_freep(&value);
    avio_seek(s->pb, off + len, SEEK_SET);
}
