static int mxf_read_pulldown_component(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFPulldownComponent *mxf_pulldown = arg;
    switch(tag) {
    case 0x0d01:
        avio_read(pb, mxf_pulldown->input_segment_ref, 16);
        break;
    }
    return 0;
}
