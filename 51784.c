static gboolean verify_header_crc(tvbuff_t * tvb, packet_info * pinfo, proto_item * pi, guint16 header_crc, guint header_length)
{
    guint8 crc = 0;
    guint8 * data = NULL;
    /* Get data of header with first byte removed. */
    data = (guint8 *)tvb_memdup(wmem_packet_scope(), tvb, 1, header_length-1);
    /* Calculate crc7 sum. */
    crc = crc7update(0, data, header_length-1);
    crc = crc7finalize(crc); /* finalize crc */
    if (header_crc == crc) {
        proto_item_append_text(pi, " [correct]");
        return TRUE;
    } else {
        proto_item_append_text(pi, " [incorrect, should be 0x%x]", crc);
        expert_add_info(pinfo, pi, &ei_fp_bad_header_checksum);
        return FALSE;
    }
}
