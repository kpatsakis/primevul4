static void mxf_compute_essence_containers(MXFContext *mxf)
{
    int x;

    /* everything is already correct */
    if (mxf->op == OPAtom)
        return;

    for (x = 0; x < mxf->partitions_count; x++) {
        MXFPartition *p = &mxf->partitions[x];

        if (!p->body_sid)
            continue;       /* BodySID == 0 -> no essence */

        if (x >= mxf->partitions_count - 1)
            break;          /* FooterPartition - can't compute length (and we don't need to) */

        /* essence container spans to the next partition */
        p->essence_length = mxf->partitions[x+1].this_partition - p->essence_offset;

        if (p->essence_length < 0) {
            /* next ThisPartition < essence_offset */
            p->essence_length = 0;
            av_log(mxf->fc, AV_LOG_ERROR,
                   "partition %i: bad ThisPartition = %"PRIX64"\n",
                   x+1, mxf->partitions[x+1].this_partition);
        }
    }
}
