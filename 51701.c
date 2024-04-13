dissect_80211n_mac(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, int offset, int data_len, gboolean add_subtree, guint32 *n_mac_flags, guint32 *ampdu_id, struct ieee_802_11_phdr *phdr)
{
    proto_tree  *ftree       = tree;
    ptvcursor_t *csr;
    int          subtree_off = add_subtree ? 4 : 0;
    guint32      flags;

    phdr->phy = PHDR_802_11_PHY_11N;

    *n_mac_flags = tvb_get_letohl(tvb, offset + subtree_off);
    *ampdu_id = tvb_get_letohl(tvb, offset + 4 + subtree_off);

    if (add_subtree) {
        ftree = proto_tree_add_subtree(tree, tvb, offset, data_len, ett_dot11n_mac, NULL, "802.11n MAC");
        add_ppi_field_header(tvb, ftree, &offset);
        data_len -= 4; /* Subtract field header length */
    }

    if (data_len != PPI_80211N_MAC_LEN) {
        proto_tree_add_expert_format(ftree, pinfo, &ei_ppi_invalid_length, tvb, offset, data_len, "Invalid length: %u", data_len);
        THROW(ReportedBoundsError);
    }

    csr = ptvcursor_new(ftree, tvb, offset);

    flags = tvb_get_letohl(tvb, ptvcursor_current_offset(csr));
    phdr->phy_info.info_11n.presence_flags = PHDR_802_11N_HAS_SHORT_GI|PHDR_802_11N_HAS_GREENFIELD;
    phdr->phy_info.info_11n.short_gi = ((flags & DOT11N_FLAG_SHORT_GI) != 0);
    phdr->phy_info.info_11n.greenfield = ((flags & DOT11N_FLAG_GREENFIELD) != 0);
    ptvcursor_add_with_subtree(csr, hf_80211n_mac_flags, 4, ENC_LITTLE_ENDIAN,
                               ett_dot11n_mac_flags);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_flags_greenfield, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_flags_ht20_40, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_flags_rx_guard_interval, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_flags_duplicate_rx, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_flags_aggregate, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_flags_more_aggregates, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add(csr, hf_80211n_mac_flags_delimiter_crc_after, 4, ENC_LITTLE_ENDIAN); /* Last */
    ptvcursor_pop_subtree(csr);

    ptvcursor_add(csr, hf_80211n_mac_ampdu_id, 4, ENC_LITTLE_ENDIAN);
    ptvcursor_add(csr, hf_80211n_mac_num_delimiters, 1, ENC_LITTLE_ENDIAN);

    if (add_subtree) {
        ptvcursor_add(csr, hf_80211n_mac_reserved, 3, ENC_LITTLE_ENDIAN);
    }

    ptvcursor_free(csr);
}
