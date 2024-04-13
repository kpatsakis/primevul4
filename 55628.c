static int mxf_parse_handle_essence(MXFContext *mxf)
{
    AVIOContext *pb = mxf->fc->pb;
    int64_t ret;

    if (mxf->parsing_backward) {
        return mxf_seek_to_previous_partition(mxf);
    } else {
        if (!mxf->footer_partition) {
            av_log(mxf->fc, AV_LOG_TRACE, "no FooterPartition\n");
            return 0;
        }

        av_log(mxf->fc, AV_LOG_TRACE, "seeking to FooterPartition\n");

        /* remember where we were so we don't end up seeking further back than this */
        mxf->last_forward_tell = avio_tell(pb);

        if (!(pb->seekable & AVIO_SEEKABLE_NORMAL)) {
            av_log(mxf->fc, AV_LOG_INFO, "file is not seekable - not parsing FooterPartition\n");
            return -1;
        }

        /* seek to FooterPartition and parse backward */
        if ((ret = avio_seek(pb, mxf->run_in + mxf->footer_partition, SEEK_SET)) < 0) {
            av_log(mxf->fc, AV_LOG_ERROR,
                   "failed to seek to FooterPartition @ 0x%" PRIx64
                   " (%"PRId64") - partial file?\n",
                   mxf->run_in + mxf->footer_partition, ret);
            return ret;
        }

        mxf->current_partition = NULL;
        mxf->parsing_backward = 1;
    }

    return 1;
}
