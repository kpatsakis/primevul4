INT AirPDcapDestroyContext(
    PAIRPDCAP_CONTEXT ctx)
{
    AIRPDCAP_DEBUG_TRACE_START("AirPDcapDestroyContext");

    if (ctx==NULL) {
        AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapDestroyContext", "NULL context", AIRPDCAP_DEBUG_LEVEL_5);
        AIRPDCAP_DEBUG_TRACE_END("AirPDcapDestroyContext");
        return AIRPDCAP_RET_UNSUCCESS;
    }

    AirPDcapCleanKeys(ctx);
    AirPDcapCleanSecAssoc(ctx);

    ctx->first_free_index=0;
    ctx->index=-1;
    ctx->sa_index=-1;

    AIRPDCAP_DEBUG_PRINT_LINE("AirPDcapDestroyContext", "Context destroyed!", AIRPDCAP_DEBUG_LEVEL_5);
    AIRPDCAP_DEBUG_TRACE_END("AirPDcapDestroyContext");
    return AIRPDCAP_RET_SUCCESS;
}
