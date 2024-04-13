static void uat_umts_fp_record_free_cb(void*r) {
    uat_umts_fp_ep_and_ch_record_t *rec = (uat_umts_fp_ep_and_ch_record_t *)r;

    g_free(rec->srcIP);
    g_free(rec->dstIP);
}
