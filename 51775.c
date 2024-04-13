heur_dissect_fp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
    struct fp_info *p_fp_info;

    p_fp_info = (fp_info *)p_get_proto_data(wmem_file_scope(), pinfo, proto_fp, 0);

    /* if no FP info is present, this might be FP in a pcap(ng) file */
    if (!p_fp_info) {
        /* We only know the header length of control frames, so check that bit first */
        int offset = 0, length;
        guint8 oct, calc_crc = 0, crc;
        unsigned char *buf;

        oct = tvb_get_guint8(tvb, offset);
        crc = oct & 0xfe;
        if ((oct & 0x01) == 1) {
            /*
             * 6.3.2.1 Frame CRC
             * Description: It is the result of the CRC applied to the remaining part of the frame,
             * i.e. from bit 0 of the first byte of the header (the FT IE) to bit 0 of the last byte of the payload,
             * with the corresponding generator polynomial: G(D) = D7+D6+D2+1. See subclause 7.2.
             */
            length =  tvb_reported_length(tvb);
            buf = (unsigned char *)tvb_memdup(wmem_packet_scope(), tvb, 0, length);
            buf[0] = 01;

            calc_crc = crc7update(calc_crc, buf, length);
            if (calc_crc == crc) {
                /* assume this is FP, set conversatio dissector to catch the data frames too */
                conversation_set_dissector(find_or_create_conversation(pinfo), fp_handle);
                dissect_fp(tvb, pinfo, tree, data);
                return TRUE;
            }
        }
        return FALSE;
    }

    /* if FP info is present, check that it really is an ethernet link */
    if (p_fp_info->link_type != FP_Link_Ethernet) {
        return FALSE;
    }

    /* discriminate 'lower' UDP layer from 'user data' UDP layer
     * (i.e. if an FP over UDP packet contains a user UDP packet */
    if (p_fp_info->srcport != pinfo->srcport ||
        p_fp_info->destport != pinfo->destport)
        return FALSE;

    /* assume this is FP */
    dissect_fp(tvb, pinfo, tree, data);
    return TRUE;
}
