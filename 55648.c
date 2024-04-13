static int mxf_read_preface_metadata(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFContext *mxf = arg;
    AVFormatContext *s = mxf->fc;
    int ret;
    char *str = NULL;

    if (tag >= 0x8000 && (IS_KLV_KEY(uid, mxf_avid_project_name))) {
        SET_STR_METADATA(pb, "project_name", str);
    }
    return 0;
}
