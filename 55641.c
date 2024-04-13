static int mxf_read_index_table_segment(void *arg, AVIOContext *pb, int tag, int size, UID uid, int64_t klv_offset)
{
    MXFIndexTableSegment *segment = arg;
    switch(tag) {
    case 0x3F05:
        segment->edit_unit_byte_count = avio_rb32(pb);
        av_log(NULL, AV_LOG_TRACE, "EditUnitByteCount %d\n", segment->edit_unit_byte_count);
        break;
    case 0x3F06:
        segment->index_sid = avio_rb32(pb);
        av_log(NULL, AV_LOG_TRACE, "IndexSID %d\n", segment->index_sid);
        break;
    case 0x3F07:
        segment->body_sid = avio_rb32(pb);
        av_log(NULL, AV_LOG_TRACE, "BodySID %d\n", segment->body_sid);
        break;
    case 0x3F0A:
        av_log(NULL, AV_LOG_TRACE, "IndexEntryArray found\n");
        return mxf_read_index_entry_array(pb, segment);
    case 0x3F0B:
        segment->index_edit_rate.num = avio_rb32(pb);
        segment->index_edit_rate.den = avio_rb32(pb);
        av_log(NULL, AV_LOG_TRACE, "IndexEditRate %d/%d\n", segment->index_edit_rate.num,
                segment->index_edit_rate.den);
        break;
    case 0x3F0C:
        segment->index_start_position = avio_rb64(pb);
        av_log(NULL, AV_LOG_TRACE, "IndexStartPosition %"PRId64"\n", segment->index_start_position);
        break;
    case 0x3F0D:
        segment->index_duration = avio_rb64(pb);
        av_log(NULL, AV_LOG_TRACE, "IndexDuration %"PRId64"\n", segment->index_duration);
        break;
    }
    return 0;
}
