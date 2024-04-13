dissect_80211n_mac_phy(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset, int data_len, guint32 *n_mac_flags, guint32 *ampdu_id, struct ieee_802_11_phdr *phdr)
{
    proto_tree  *ftree;
    proto_item  *ti;
    ptvcursor_t *csr;
    guint8       mcs;
    guint8       ness;
    guint16      ext_frequency;
    gchar       *chan_str;

    ftree = proto_tree_add_subtree(tree, tvb, offset, data_len, ett_dot11n_mac_phy, NULL, "802.11n MAC+PHY");
    add_ppi_field_header(tvb, ftree, &offset);
    data_len -= 4; /* Subtract field header length */

    if (data_len != PPI_80211N_MAC_PHY_LEN) {
        proto_tree_add_expert_format(ftree, pinfo, &ei_ppi_invalid_length, tvb, offset, data_len, "Invalid length: %u", data_len);
        THROW(ReportedBoundsError);
    }

    dissect_80211n_mac(tvb, pinfo, ftree, offset, PPI_80211N_MAC_LEN,
                       FALSE, n_mac_flags, ampdu_id, phdr);
    offset += PPI_80211N_MAC_PHY_OFF;

    csr = ptvcursor_new(ftree, tvb, offset);

    mcs = tvb_get_guint8(tvb, ptvcursor_current_offset(csr));
    if (mcs != 255) {
        phdr->phy_info.info_11n.presence_flags |= PHDR_802_11N_HAS_MCS_INDEX;
        phdr->phy_info.info_11n.mcs_index = mcs;
    }
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_mcs, 1, 255);

    ness = tvb_get_guint8(tvb, ptvcursor_current_offset(csr));
    phdr->phy_info.info_11n.presence_flags |= PHDR_802_11N_HAS_NESS;
    phdr->phy_info.info_11n.ness = ness;
    ti = ptvcursor_add(csr, hf_80211n_mac_phy_num_streams, 1, ENC_LITTLE_ENDIAN);
    if (tvb_get_guint8(tvb, ptvcursor_current_offset(csr) - 1) == 0)
        proto_item_append_text(ti, " (unknown)");
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_combined, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant0_ctl, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant1_ctl, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant2_ctl, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant3_ctl, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant0_ext, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant1_ext, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant2_ext, 1, 255);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_rssi_ant3_ext, 1, 255);

    ext_frequency = tvb_get_letohs(ptvcursor_tvbuff(csr), ptvcursor_current_offset(csr));
    chan_str = ieee80211_mhz_to_str(ext_frequency);
    proto_tree_add_uint_format(ptvcursor_tree(csr), hf_80211n_mac_phy_ext_chan_freq, ptvcursor_tvbuff(csr),
                               ptvcursor_current_offset(csr), 2, ext_frequency, "Ext. Channel frequency: %s", chan_str);
    g_free(chan_str);
    ptvcursor_advance(csr, 2);

    ptvcursor_add_with_subtree(csr, hf_80211n_mac_phy_ext_chan_flags, 2, ENC_LITTLE_ENDIAN,
                               ett_dot11n_mac_phy_ext_channel_flags);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_phy_ext_chan_flags_turbo, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_phy_ext_chan_flags_cck, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_phy_ext_chan_flags_ofdm, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_phy_ext_chan_flags_2ghz, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_phy_ext_chan_flags_5ghz, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_phy_ext_chan_flags_passive, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211n_mac_phy_ext_chan_flags_dynamic, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add(csr, hf_80211n_mac_phy_ext_chan_flags_gfsk, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_pop_subtree(csr);

    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant0signal, 1, 0x80); /* -128 */
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant0noise, 1, 0x80);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant1signal, 1, 0x80);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant1noise, 1, 0x80);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant2signal, 1, 0x80);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant2noise, 1, 0x80);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant3signal, 1, 0x80);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_dbm_ant3noise, 1, 0x80);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_evm0, 4, 0);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_evm1, 4, 0);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_evm2, 4, 0);
    ptvcursor_add_invalid_check(csr, hf_80211n_mac_phy_evm3, 4, 0);

    ptvcursor_free(csr);
}
