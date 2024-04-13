capture_ppi(const guchar *pd, int len, packet_counts *ld)
{
    guint32  dlt;
    guint    ppi_len;

    ppi_len = pletoh16(pd+2);
    if(ppi_len < PPI_V0_HEADER_LEN || !BYTES_ARE_IN_FRAME(0, len, ppi_len)) {
        ld->other++;
        return;
    }

    dlt = pletoh32(pd+4);

    /* XXX - We should probably combine this with capture_info.c:capture_info_packet() */
    switch(dlt) {
        case 1: /* DLT_EN10MB */
            capture_eth(pd, ppi_len, len, ld);
            return;
        case 105: /* DLT_DLT_IEEE802_11 */
            capture_ieee80211(pd, ppi_len, len, ld);
            return;
        default:
            break;
    }

    ld->other++;
}
