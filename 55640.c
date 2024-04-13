static int mxf_read_identification_metadata(void *arg, AVIOContext *pb, int tag, int size, UID _uid, int64_t klv_offset)
{
    MXFContext *mxf = arg;
    AVFormatContext *s = mxf->fc;
    int ret;
    UID uid = { 0 };
    char *str = NULL;
    uint64_t ts;
    switch (tag) {
    case 0x3C01:
        SET_STR_METADATA(pb, "company_name", str);
        break;
    case 0x3C02:
        SET_STR_METADATA(pb, "product_name", str);
        break;
    case 0x3C04:
        SET_STR_METADATA(pb, "product_version", str);
        break;
    case 0x3C05:
        SET_UID_METADATA(pb, "product_uid", uid, str);
        break;
    case 0x3C06:
        SET_TS_METADATA(pb, "modification_date", ts, str);
        break;
    case 0x3C08:
        SET_STR_METADATA(pb, "application_platform", str);
        break;
    case 0x3C09:
        SET_UID_METADATA(pb, "generation_uid", uid, str);
        break;
    case 0x3C0A:
        SET_UID_METADATA(pb, "uid", uid, str);
        break;
    }
    return 0;
}
