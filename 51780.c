static void *uat_umts_fp_record_copy_cb(void *n, const void *o, size_t siz _U_) {
    uat_umts_fp_ep_and_ch_record_t *new_rec = (uat_umts_fp_ep_and_ch_record_t *)n;
    const uat_umts_fp_ep_and_ch_record_t *old_rec = (const uat_umts_fp_ep_and_ch_record_t *)o;

    new_rec->srcIP = (old_rec->srcIP) ? g_strdup(old_rec->srcIP) : NULL;
    new_rec->dstIP = (old_rec->dstIP) ? g_strdup(old_rec->dstIP) : NULL;

    return new_rec;
}
