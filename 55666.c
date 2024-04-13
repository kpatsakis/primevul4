static int64_t mxf_set_current_edit_unit(MXFContext *mxf, int64_t current_offset)
{
    int64_t last_ofs = -1, next_ofs = -1;
    MXFIndexTable *t = &mxf->index_tables[0];

    /* this is called from the OP1a demuxing logic, which means there
     * may be no index tables */
    if (mxf->nb_index_tables <= 0)
        return -1;

    /* find mxf->current_edit_unit so that the next edit unit starts ahead of current_offset */
    while (mxf->current_edit_unit >= 0) {
        if (mxf_edit_unit_absolute_offset(mxf, t, mxf->current_edit_unit + 1, NULL, &next_ofs, 0) < 0)
            return -1;

        if (next_ofs <= last_ofs) {
            /* large next_ofs didn't change or current_edit_unit wrapped
             * around this fixes the infinite loop on zzuf3.mxf */
            av_log(mxf->fc, AV_LOG_ERROR,
                   "next_ofs didn't change. not deriving packet timestamps\n");
            return -1;
        }

        if (next_ofs > current_offset)
            break;

        last_ofs = next_ofs;
        mxf->current_edit_unit++;
    }

    /* not checking mxf->current_edit_unit >= t->nb_ptses here since CBR files may lack IndexEntryArrays */
    if (mxf->current_edit_unit < 0)
        return -1;

    return next_ofs;
}
