static int mxf_read_timecode_component(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFTimecodeComponent *mxf_timecode = arg;
    switch(tag) {
    case 0x1501:
        mxf_timecode->start_frame = avio_rb64(pb);
        break;
    case 0x1502:
        mxf_timecode->rate = (AVRational){avio_rb16(pb), 1};
        break;
    case 0x1503:
        mxf_timecode->drop_frame = avio_r8(pb);
        break;
    }
    return 0;
}
