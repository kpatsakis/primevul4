static gboolean verify_control_frame_crc(tvbuff_t * tvb, packet_info * pinfo, proto_item * pi, guint16 frame_crc)
{
    guint8 crc = 0;
    guint8 * data = NULL;
    /* Get data. */
    data = (guint8 *)tvb_memdup(wmem_packet_scope(), tvb, 0, tvb_reported_length(tvb));
    /* Include only FT flag bit in CRC calculation. */
    data[0] = data[0] & 1;
    /* Calculate crc7 sum. */
    crc = crc7update(0, data, tvb_reported_length(tvb));
    crc = crc7finalize(crc); /* finalize crc */
    if (frame_crc == crc) {
        proto_item_append_text(pi, " [correct]");
        return TRUE;
    } else {
        proto_item_append_text(pi, " [incorrect, should be 0x%x]", crc);
        expert_add_info(pinfo, pi, &ei_fp_bad_header_checksum);
        return FALSE;
    }
}
