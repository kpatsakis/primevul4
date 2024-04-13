AirPDcapGetSaPtr(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
{
    int sa_index;

    /* search for a cached Security Association for supplied BSSID and STA MAC  */
    if ((sa_index=AirPDcapGetSa(ctx, id))==-1) {
        /* create a new Security Association if it doesn't currently exist      */
        if ((sa_index=AirPDcapStoreSa(ctx, id))==-1) {
            return NULL;
        }
    }
    /* get the Security Association structure   */
    return &ctx->sa[sa_index];
}
