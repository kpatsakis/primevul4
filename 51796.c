AirPDcapRecurseCleanSA(
    PAIRPDCAP_SEC_ASSOCIATION sa)
{
    if (sa->next != NULL) {
        AirPDcapRecurseCleanSA(sa->next);
        g_free(sa->next);
        sa->next = NULL;
    }
}
