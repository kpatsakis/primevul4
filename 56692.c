static void amf_tag_contents(void *ctx, const uint8_t *data,
                             const uint8_t *data_end)
{
    unsigned int size, nb = -1;
    char buf[1024];
    AMFDataType type;
    int parse_key = 1;

    if (data >= data_end)
        return;
    switch ((type = *data++)) {
    case AMF_DATA_TYPE_NUMBER:
        av_log(ctx, AV_LOG_DEBUG, " number %g\n", av_int2double(AV_RB64(data)));
        return;
    case AMF_DATA_TYPE_BOOL:
        av_log(ctx, AV_LOG_DEBUG, " bool %d\n", *data);
        return;
    case AMF_DATA_TYPE_STRING:
    case AMF_DATA_TYPE_LONG_STRING:
        if (type == AMF_DATA_TYPE_STRING) {
            size = bytestream_get_be16(&data);
        } else {
            size = bytestream_get_be32(&data);
        }
        size = FFMIN(size, sizeof(buf) - 1);
        memcpy(buf, data, size);
        buf[size] = 0;
        av_log(ctx, AV_LOG_DEBUG, " string '%s'\n", buf);
        return;
    case AMF_DATA_TYPE_NULL:
        av_log(ctx, AV_LOG_DEBUG, " NULL\n");
        return;
    case AMF_DATA_TYPE_ARRAY:
        parse_key = 0;
    case AMF_DATA_TYPE_MIXEDARRAY:
        nb = bytestream_get_be32(&data);
    case AMF_DATA_TYPE_OBJECT:
        av_log(ctx, AV_LOG_DEBUG, " {\n");
        while (nb-- > 0 || type != AMF_DATA_TYPE_ARRAY) {
            int t;
            if (parse_key) {
                size = bytestream_get_be16(&data);
                size = FFMIN(size, sizeof(buf) - 1);
                if (!size) {
                    av_log(ctx, AV_LOG_DEBUG, " }\n");
                    data++;
                    break;
                }
                memcpy(buf, data, size);
                buf[size] = 0;
                if (size >= data_end - data)
                    return;
                data += size;
                av_log(ctx, AV_LOG_DEBUG, "  %s: ", buf);
            }
            amf_tag_contents(ctx, data, data_end);
            t = ff_amf_tag_size(data, data_end);
            if (t < 0 || t >= data_end - data)
                return;
            data += t;
        }
        return;
    case AMF_DATA_TYPE_OBJECT_END:
        av_log(ctx, AV_LOG_DEBUG, " }\n");
        return;
    default:
        return;
    }
