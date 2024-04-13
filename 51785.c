static gboolean verify_header_crc_edch(tvbuff_t * tvb, packet_info * pinfo, proto_item * pi, guint16 header_crc, guint header_length)
{
    guint16 crc = 0;
    guint8 * data = NULL;
    /* First create new subset of header with first byte removed. */
    tvbuff_t * headtvb = tvb_new_subset_length(tvb, 1, header_length-1);
    /* Get data of header with first byte removed. */
    data = (guint8 *)tvb_memdup(wmem_packet_scope(), headtvb, 0, header_length-1);
    /* Remove first 4 bits of the remaining data which are Header CRC cont. */
    data[0] = data[0] & 0x0f;
    crc = crc11_307_noreflect_noxor(data, header_length-1);
    if (header_crc == crc) {
        proto_item_append_text(pi, " [correct]");
        return TRUE;
    } else {
        proto_item_append_text(pi, " [incorrect, should be 0x%x]", crc);
        expert_add_info(pinfo, pi, &ei_fp_bad_header_checksum);
        return FALSE;
    }
}
