static int amf_tag_skip(GetByteContext *gb)
{
    AMFDataType type;
    unsigned nb   = -1;
    int parse_key = 1;

    if (bytestream2_get_bytes_left(gb) < 1)
        return -1;

    type = bytestream2_get_byte(gb);
    switch (type) {
    case AMF_DATA_TYPE_NUMBER:
        bytestream2_get_be64(gb);
        return 0;
    case AMF_DATA_TYPE_BOOL:
        bytestream2_get_byte(gb);
        return 0;
    case AMF_DATA_TYPE_STRING:
        bytestream2_skip(gb, bytestream2_get_be16(gb));
        return 0;
    case AMF_DATA_TYPE_LONG_STRING:
        bytestream2_skip(gb, bytestream2_get_be32(gb));
        return 0;
    case AMF_DATA_TYPE_NULL:
        return 0;
    case AMF_DATA_TYPE_DATE:
        bytestream2_skip(gb, 10);
        return 0;
    case AMF_DATA_TYPE_ARRAY:
        parse_key = 0;
    case AMF_DATA_TYPE_MIXEDARRAY:
        nb = bytestream2_get_be32(gb);
    case AMF_DATA_TYPE_OBJECT:
        while (nb-- > 0 || type != AMF_DATA_TYPE_ARRAY) {
            int t;
            if (parse_key) {
                int size = bytestream2_get_be16(gb);
                if (!size) {
                    bytestream2_get_byte(gb);
                    break;
                }
                if (size < 0 || size >= bytestream2_get_bytes_left(gb))
                    return -1;
                bytestream2_skip(gb, size);
            }
            t = amf_tag_skip(gb);
            if (t < 0 || bytestream2_get_bytes_left(gb) <= 0)
                return -1;
        }
        return 0;
    case AMF_DATA_TYPE_OBJECT_END:  return 0;
    default:                        return -1;
    }
}
