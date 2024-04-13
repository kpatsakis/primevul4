dissect_80211_common(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset, int data_len, struct ieee_802_11_phdr *phdr)
{
    proto_tree  *ftree;
    proto_item  *ti;
    ptvcursor_t *csr;
    guint64      tsft_raw;
    guint        rate_raw;
    guint        rate_kbps;
    guint32      common_flags;
    guint16      common_frequency;
    guint16      chan_flags;
    gint8        dbm_value;
    gchar       *chan_str;

    ftree = proto_tree_add_subtree(tree, tvb, offset, data_len, ett_dot11_common, NULL, "802.11-Common");
    add_ppi_field_header(tvb, ftree, &offset);
    data_len -= 4; /* Subtract field header length */

    if (data_len != PPI_80211_COMMON_LEN) {
        proto_tree_add_expert_format(ftree, pinfo, &ei_ppi_invalid_length, tvb, offset, data_len, "Invalid length: %u", data_len);
        THROW(ReportedBoundsError);
    }

    common_flags = tvb_get_letohs(tvb, offset + 8);
    if (common_flags & DOT11_FLAG_HAVE_FCS)
        phdr->fcs_len = 4;
    else
        phdr->fcs_len = 0;

    csr = ptvcursor_new(ftree, tvb, offset);

    tsft_raw = tvb_get_letoh64(tvb, offset);
    if (tsft_raw != 0) {
        phdr->presence_flags |= PHDR_802_11_HAS_TSF_TIMESTAMP;
        if (common_flags & DOT11_FLAG_TSF_TIMER_MS)
            phdr->tsf_timestamp = tsft_raw * 1000;
        else
            phdr->tsf_timestamp = tsft_raw;
    }

    ptvcursor_add_invalid_check(csr, hf_80211_common_tsft, 8, 0);

    ptvcursor_add_with_subtree(csr, hf_80211_common_flags, 2, ENC_LITTLE_ENDIAN,
                               ett_dot11_common_flags);
    ptvcursor_add_no_advance(csr, hf_80211_common_flags_fcs, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_flags_tsft, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_flags_fcs_valid, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add(csr, hf_80211_common_flags_phy_err, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_pop_subtree(csr);

    rate_raw = tvb_get_letohs(tvb, ptvcursor_current_offset(csr));
    if (rate_raw != 0) {
        phdr->presence_flags |= PHDR_802_11_HAS_DATA_RATE;
        phdr->data_rate = rate_raw;
    }
    rate_kbps = rate_raw * 500;
    ti = proto_tree_add_uint_format(ftree, hf_80211_common_rate, tvb,
                                    ptvcursor_current_offset(csr), 2, rate_kbps, "Rate: %.1f Mbps",
                                    rate_kbps / 1000.0);
    if (rate_kbps == 0)
        proto_item_append_text(ti, " [invalid]");
    col_add_fstr(pinfo->cinfo, COL_TX_RATE, "%.1f Mbps", rate_kbps / 1000.0);
    ptvcursor_advance(csr, 2);

    common_frequency = tvb_get_letohs(ptvcursor_tvbuff(csr), ptvcursor_current_offset(csr));
    if (common_frequency != 0) {
        gint calc_channel;

        phdr->presence_flags |= PHDR_802_11_HAS_FREQUENCY;
        phdr->frequency = common_frequency;
        calc_channel = ieee80211_mhz_to_chan(common_frequency);
        if (calc_channel != -1) {
            phdr->presence_flags |= PHDR_802_11_HAS_CHANNEL;
            phdr->channel = calc_channel;
        }
    }
    chan_str = ieee80211_mhz_to_str(common_frequency);
    proto_tree_add_uint_format_value(ptvcursor_tree(csr), hf_80211_common_chan_freq, ptvcursor_tvbuff(csr),
                               ptvcursor_current_offset(csr), 2, common_frequency, "%s", chan_str);
    col_add_fstr(pinfo->cinfo, COL_FREQ_CHAN, "%s", chan_str);
    g_free(chan_str);
    ptvcursor_advance(csr, 2);

    chan_flags = tvb_get_letohs(ptvcursor_tvbuff(csr), ptvcursor_current_offset(csr));
    switch (chan_flags & IEEE80211_CHAN_ALLTURBO) {

    case IEEE80211_CHAN_FHSS:
        phdr->phy = PHDR_802_11_PHY_11_FHSS;
        phdr->phy_info.info_11_fhss.presence_flags =
            PHDR_802_11_FHSS_HAS_HOP_SET |
            PHDR_802_11_FHSS_HAS_HOP_PATTERN;
        break;

    case IEEE80211_CHAN_DSSS:
        phdr->phy = PHDR_802_11_PHY_11_DSSS;
        break;

    case IEEE80211_CHAN_A:
        phdr->phy = PHDR_802_11_PHY_11A;
        phdr->phy_info.info_11a.presence_flags = PHDR_802_11A_HAS_TURBO_TYPE;
        phdr->phy_info.info_11a.turbo_type = PHDR_802_11A_TURBO_TYPE_NORMAL;
        break;

    case IEEE80211_CHAN_B:
        phdr->phy = PHDR_802_11_PHY_11B;
        phdr->phy_info.info_11b.presence_flags = 0;
        break;

    case IEEE80211_CHAN_PUREG:
        phdr->phy = PHDR_802_11_PHY_11G;
        phdr->phy_info.info_11g.presence_flags = PHDR_802_11G_HAS_MODE;
        phdr->phy_info.info_11g.mode = PHDR_802_11G_MODE_NORMAL;
        break;

    case IEEE80211_CHAN_G:
        phdr->phy = PHDR_802_11_PHY_11G;
        phdr->phy_info.info_11g.presence_flags = PHDR_802_11G_HAS_MODE;
        phdr->phy_info.info_11g.mode = PHDR_802_11G_MODE_NORMAL;
        break;

    case IEEE80211_CHAN_108A:
        phdr->phy = PHDR_802_11_PHY_11A;
        phdr->phy_info.info_11a.presence_flags = PHDR_802_11A_HAS_TURBO_TYPE;
        /* We assume non-STURBO is dynamic turbo */
        phdr->phy_info.info_11a.turbo_type = PHDR_802_11A_TURBO_TYPE_DYNAMIC_TURBO;
        break;

    case IEEE80211_CHAN_108PUREG:
        phdr->phy = PHDR_802_11_PHY_11G;
        phdr->phy_info.info_11g.presence_flags = PHDR_802_11G_HAS_MODE;
        phdr->phy_info.info_11g.mode = PHDR_802_11G_MODE_SUPER_G;
        break;
    }
    ptvcursor_add_with_subtree(csr, hf_80211_common_chan_flags, 2, ENC_LITTLE_ENDIAN,
                               ett_dot11_common_channel_flags);
    ptvcursor_add_no_advance(csr, hf_80211_common_chan_flags_turbo, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_chan_flags_cck, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_chan_flags_ofdm, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_chan_flags_2ghz, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_chan_flags_5ghz, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_chan_flags_passive, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add_no_advance(csr, hf_80211_common_chan_flags_dynamic, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_add(csr, hf_80211_common_chan_flags_gfsk, 2, ENC_LITTLE_ENDIAN);
    ptvcursor_pop_subtree(csr);


    if (phdr->phy == PHDR_802_11_PHY_11_FHSS)
        phdr->phy_info.info_11_fhss.hop_set = tvb_get_guint8(ptvcursor_tvbuff(csr), ptvcursor_current_offset(csr));
    ptvcursor_add(csr, hf_80211_common_fhss_hopset, 1, ENC_LITTLE_ENDIAN);
    if (phdr->phy == PHDR_802_11_PHY_11_FHSS)
        phdr->phy_info.info_11_fhss.hop_pattern = tvb_get_guint8(ptvcursor_tvbuff(csr), ptvcursor_current_offset(csr));
    ptvcursor_add(csr, hf_80211_common_fhss_pattern, 1, ENC_LITTLE_ENDIAN);

    dbm_value = (gint8) tvb_get_guint8(tvb, ptvcursor_current_offset(csr));
    if (dbm_value != -128 && dbm_value != 0) {
        /*
         * XXX - the spec says -128 is invalid, presumably meaning "use
         * -128 if you don't have the signal strength", but some captures
         * have 0 for noise, presumably meaning it's incorrectly being
         * used for "don't have it", so we check for it as well.
         */
        col_add_fstr(pinfo->cinfo, COL_RSSI, "%d dBm", dbm_value);
        phdr->presence_flags |= PHDR_802_11_HAS_SIGNAL_DBM;
        phdr->signal_dbm = dbm_value;
    }
    ptvcursor_add_invalid_check(csr, hf_80211_common_dbm_antsignal, 1, 0x80); /* -128 */

    dbm_value = (gint8) tvb_get_guint8(tvb, ptvcursor_current_offset(csr));
    if (dbm_value != -128 && dbm_value != 0) {
        /*
         * XXX - the spec says -128 is invalid, presumably meaning "use
         * -128 if you don't have the noise level", but some captures
         * have 0, presumably meaning it's incorrectly being used for
         * "don't have it", so we check for it as well.
         */
        phdr->presence_flags |= PHDR_802_11_HAS_NOISE_DBM;
        phdr->noise_dbm = dbm_value;
    }
    ptvcursor_add_invalid_check(csr, hf_80211_common_dbm_antnoise, 1, 0x80);

    ptvcursor_free(csr);
}
