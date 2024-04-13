static int nsv_read_header(AVFormatContext *s)
{
    NSVContext *nsv = s->priv_data;
    int i, err;

    nsv->state = NSV_UNSYNC;
    nsv->ahead[0].data = nsv->ahead[1].data = NULL;

    for (i = 0; i < NSV_MAX_RESYNC_TRIES; i++) {
        if (nsv_resync(s) < 0)
            return -1;
        if (nsv->state == NSV_FOUND_NSVF) {
            err = nsv_parse_NSVf_header(s);
            if (err < 0)
                return err;
        }
            /* we need the first NSVs also... */
        if (nsv->state == NSV_FOUND_NSVS) {
            err = nsv_parse_NSVs_header(s);
            if (err < 0)
                return err;
            break; /* we just want the first one */
        }
    }
    if (s->nb_streams < 1) /* no luck so far */
        return -1;
    /* now read the first chunk, so we can attempt to decode more info */
    err = nsv_read_chunk(s, 1);

    av_log(s, AV_LOG_TRACE, "parsed header\n");
    return err;
}
