static int mxf_parse_handle_partition_or_eof(MXFContext *mxf)
{
    return mxf->parsing_backward ? mxf_seek_to_previous_partition(mxf) : 1;
}
