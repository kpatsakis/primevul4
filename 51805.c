AirPDcapStoreSa(
    PAIRPDCAP_CONTEXT ctx,
    AIRPDCAP_SEC_ASSOCIATION_ID *id)
{
    INT last_free;
    if (ctx->first_free_index>=AIRPDCAP_MAX_SEC_ASSOCIATIONS_NR) {
        /* there is no empty space available. FAILURE */
        return -1;
    }
    if (ctx->sa[ctx->first_free_index].used) {
        /* last addition was in the middle of the array (and the first_free_index was just incremented by 1)   */
        /* search for a free space from the first_free_index to AIRPDCAP_STA_INFOS_NR (to avoid free blocks in */
        /*              the middle)                                                                            */
        for (last_free=ctx->first_free_index; last_free<AIRPDCAP_MAX_SEC_ASSOCIATIONS_NR; last_free++)
            if (!ctx->sa[last_free].used)
                break;

        if (last_free>=AIRPDCAP_MAX_SEC_ASSOCIATIONS_NR) {
            /* there is no empty space available. FAILURE */
            return -1;
        }

        /* store first free space index */
        ctx->first_free_index=last_free;
    }

    /* use this info */
    ctx->index=ctx->first_free_index;

    /* reset the info structure */
    memset(ctx->sa+ctx->index, 0, sizeof(AIRPDCAP_SEC_ASSOCIATION));

    ctx->sa[ctx->index].used=1;

    /* set the info structure */
    memcpy(&(ctx->sa[ctx->index].saId), id, sizeof(AIRPDCAP_SEC_ASSOCIATION_ID));

    /* increment by 1 the first_free_index (heuristic) */
    ctx->first_free_index++;

    /* set the sa_index if the added index is greater the the sa_index */
    if (ctx->index > ctx->sa_index)
        ctx->sa_index=ctx->index;

    return ctx->index;
}
