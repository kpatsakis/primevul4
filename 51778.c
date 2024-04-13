void proto_register_fp(void)
{
    static hf_register_info hf[] =
        {
            { &hf_fp_release,
              { "Release",
                "fp.release", FT_NONE, BASE_NONE, NULL, 0x0,
                "Release information", HFILL
              }
            },
            { &hf_fp_release_version,
              { "Release Version",
                "fp.release.version", FT_UINT8, BASE_DEC, NULL, 0x0,
                "3GPP Release number", HFILL
              }
            },
            { &hf_fp_release_year,
              { "Release year",
                "fp.release.year", FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_release_month,
              { "Release month",
                "fp.release.month", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_channel_type,
              { "Channel Type",
                "fp.channel-type", FT_UINT8, BASE_HEX, VALS(channel_type_vals), 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_division,
              { "Division",
                "fp.division", FT_UINT8, BASE_HEX, VALS(division_vals), 0x0,
                "Radio division type", HFILL
              }
            },
            { &hf_fp_direction,
              { "Direction",
                "fp.direction", FT_UINT8, BASE_HEX, VALS(direction_vals), 0x0,
                "Link direction", HFILL
              }
            },
            { &hf_fp_ddi_config,
              { "DDI Config",
                "fp.ddi-config", FT_STRING, BASE_NONE, NULL, 0x0,
                "DDI Config (for E-DCH)", HFILL
              }
            },
            { &hf_fp_ddi_config_ddi,
              { "DDI",
                "fp.ddi-config.ddi", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_ddi_config_macd_pdu_size,
              { "MACd PDU Size",
                "fp.ddi-config.macd-pdu-size", FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },


            { &hf_fp_header_crc,
              { "Header CRC",
                "fp.header-crc", FT_UINT8, BASE_HEX, NULL, 0xfe,
                NULL, HFILL
              }
            },
            { &hf_fp_ft,
              { "Frame Type",
                "fp.ft", FT_UINT8, BASE_HEX, VALS(data_control_vals), 0x01,
                NULL, HFILL
              }
            },
            { &hf_fp_cfn,
              { "CFN",
                "fp.cfn", FT_UINT8, BASE_DEC, NULL, 0x0,
                "Connection Frame Number", HFILL
              }
            },
            { &hf_fp_pch_cfn,
              { "CFN (PCH)",
                "fp.pch.cfn", FT_UINT16, BASE_DEC, NULL, 0xfff0,
                "PCH Connection Frame Number", HFILL
              }
            },
            { &hf_fp_pch_toa,
              { "ToA (PCH)",
                "fp.pch.toa", FT_INT24, BASE_DEC, NULL, 0x0,
                "PCH Time of Arrival", HFILL
              }
            },
            { &hf_fp_cfn_control,
              { "CFN control",
                "fp.cfn-control", FT_UINT8, BASE_DEC, NULL, 0x0,
                "Connection Frame Number Control", HFILL
              }
            },
            { &hf_fp_toa,
              { "ToA",
                "fp.toa", FT_INT16, BASE_DEC, NULL, 0x0,
                "Time of arrival (units are 125 microseconds)", HFILL
              }
            },
            { &hf_fp_tb,
              { "TB",
                "fp.tb", FT_BYTES, BASE_NONE, NULL, 0x0,
                "Transport Block", HFILL
              }
            },
            { &hf_fp_chan_zero_tbs,
              { "No TBs for channel",
                "fp.channel-with-zero-tbs", FT_UINT32, BASE_DEC, NULL, 0x0,
                "Channel with 0 TBs", HFILL
              }
            },
            { &hf_fp_tfi,
              { "TFI",
                "fp.tfi", FT_UINT8, BASE_DEC, NULL, 0x0,
                "Transport Format Indicator", HFILL
              }
            },
            { &hf_fp_usch_tfi,
              { "TFI",
                "fp.usch.tfi", FT_UINT8, BASE_DEC, NULL, 0x1f,
                "USCH Transport Format Indicator", HFILL
              }
            },
            { &hf_fp_cpch_tfi,
              { "TFI",
                "fp.cpch.tfi", FT_UINT8, BASE_DEC, NULL, 0x1f,
                "CPCH Transport Format Indicator", HFILL
              }
            },
            { &hf_fp_propagation_delay,
              { "Propagation Delay",
                "fp.propagation-delay", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_dch_control_frame_type,
              { "Control Frame Type",
                "fp.dch.control.frame-type", FT_UINT8, BASE_HEX, VALS(dch_control_frame_type_vals), 0x0,
                "DCH Control Frame Type", HFILL
              }
            },
            { &hf_fp_dch_rx_timing_deviation,
              { "Rx Timing Deviation",
                "fp.dch.control.rx-timing-deviation", FT_UINT8, BASE_DEC, 0, 0x0,
                "DCH Rx Timing Deviation", HFILL
              }
            },
            { &hf_fp_quality_estimate,
              { "Quality Estimate",
                "fp.dch.quality-estimate", FT_UINT8, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_payload_crc,
              { "Payload CRC",
                "fp.payload-crc", FT_UINT16, BASE_HEX, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_common_control_frame_type,
              { "Control Frame Type",
                "fp.common.control.frame-type", FT_UINT8, BASE_HEX, VALS(common_control_frame_type_vals), 0x0,
                "Common Control Frame Type", HFILL
              }
            },
            { &hf_fp_crci[0],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x80,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_crci[1],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x40,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_crci[2],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x20,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_crci[3],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x10,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_crci[4],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x08,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_crci[5],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x04,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_crci[6],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x02,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_crci[7],
              { "CRCI",
                "fp.crci", FT_UINT8, BASE_HEX, VALS(crci_vals), 0x01,
                "CRC correctness indicator", HFILL
              }
            },
            { &hf_fp_received_sync_ul_timing_deviation,
              { "Received SYNC UL Timing Deviation",
                "fp.rx-sync-ul-timing-deviation", FT_UINT8, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_pch_pi,
              { "Paging Indication",
                "fp.pch.pi", FT_UINT8, BASE_DEC, VALS(paging_indication_vals), 0x01,
                "Indicates if the PI Bitmap is present", HFILL
              }
            },
            { &hf_fp_pch_tfi,
              { "TFI",
                "fp.pch.tfi", FT_UINT8, BASE_DEC, 0, 0x1f,
                "PCH Transport Format Indicator", HFILL
              }
            },
            { &hf_fp_fach_tfi,
              { "TFI",
                "fp.fach.tfi", FT_UINT8, BASE_DEC, 0, 0x1f,
                "FACH Transport Format Indicator", HFILL
              }
            },
            { &hf_fp_transmit_power_level,
              { "Transmit Power Level",
                "fp.transmit-power-level", FT_FLOAT, BASE_NONE, 0, 0x0,
                "Transmit Power Level (dB)", HFILL
              }
            },
            { &hf_fp_pdsch_set_id,
              { "PDSCH Set Id",
                "fp.pdsch-set-id", FT_UINT8, BASE_DEC, 0, 0x0,
                "A pointer to the PDSCH Set which shall be used to transmit", HFILL
              }
            },
            { &hf_fp_paging_indication_bitmap,
              { "Paging Indications bitmap",
                "fp.pch.pi-bitmap", FT_NONE, BASE_NONE, NULL, 0x0,
                "Paging Indication bitmap", HFILL
              }
            },
            { &hf_fp_rx_timing_deviation,
              { "Rx Timing Deviation",
                "fp.common.control.rx-timing-deviation", FT_UINT8, BASE_DEC, 0, 0x0,
                "Common Rx Timing Deviation", HFILL
              }
            },
            { &hf_fp_dch_e_rucch_flag,
              { "E-RUCCH Flag",
                "fp.common.control.e-rucch-flag", FT_UINT8, BASE_DEC, VALS(e_rucch_flag_vals), 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_header_crc,
              { "E-DCH Header CRC",
                "fp.edch.header-crc", FT_UINT16, BASE_HEX, 0, 0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_fsn,
              { "FSN",
                "fp.edch.fsn", FT_UINT8, BASE_DEC, 0, 0x0f,
                "E-DCH Frame Sequence Number", HFILL
              }
            },
            { &hf_fp_edch_number_of_subframes,
              { "No of subframes",
                "fp.edch.no-of-subframes", FT_UINT8, BASE_DEC, 0, 0x0f,
                "E-DCH Number of subframes", HFILL
              }
            },
            { &hf_fp_edch_harq_retransmissions,
              { "No of HARQ Retransmissions",
                "fp.edch.no-of-harq-retransmissions", FT_UINT8, BASE_DEC, 0, 0x78,
                "E-DCH Number of HARQ retransmissions", HFILL
              }
            },
            { &hf_fp_edch_subframe_number,
              { "Subframe number",
                "fp.edch.subframe-number", FT_UINT8, BASE_DEC, 0, 0x0,
                "E-DCH Subframe number", HFILL
              }
            },
            { &hf_fp_edch_number_of_mac_es_pdus,
              { "Number of Mac-es PDUs",
                "fp.edch.number-of-mac-es-pdus", FT_UINT8, BASE_DEC, 0, 0xf0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_ddi,
              { "DDI",
                "fp.edch.ddi", FT_UINT8, BASE_DEC, 0, 0x0,
                "E-DCH Data Description Indicator", HFILL
              }
            },
            { &hf_fp_edch_subframe,
              { "Subframe",
                "fp.edch.subframe", FT_STRING, BASE_NONE, NULL, 0x0,
                "EDCH Subframe", HFILL
              }
            },
            { &hf_fp_edch_subframe_header,
              { "Subframe header",
                "fp.edch.subframe-header", FT_STRING, BASE_NONE, NULL, 0x0,
                "EDCH Subframe header", HFILL
              }
            },
            { &hf_fp_edch_number_of_mac_d_pdus,
              { "Number of Mac-d PDUs",
                "fp.edch.number-of-mac-d-pdus", FT_UINT8, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_pdu_padding,
              { "Padding",
                "fp.edch-data-padding", FT_UINT8, BASE_DEC, 0, 0xc0,
                "E-DCH padding before PDU", HFILL
              }
            },
            { &hf_fp_edch_tsn,
              { "TSN",
                "fp.edch-tsn", FT_UINT8, BASE_DEC, 0, 0x3f,
                "E-DCH Transmission Sequence Number", HFILL
              }
            },
            { &hf_fp_edch_mac_es_pdu,
              { "MAC-es PDU",
                "fp.edch.mac-es-pdu", FT_NONE, BASE_NONE, NULL, 0x0,
                NULL, HFILL
              }
            },

            { &hf_fp_edch_user_buffer_size,
              { "User Buffer Size",
                "fp.edch.user-buffer-size", FT_UINT24, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_no_macid_sdus,
              { "No of MAC-is SDUs",
                "fp.edch.no-macis-sdus", FT_UINT16, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_number_of_mac_is_pdus,
              { "Number of Mac-is PDUs",
                "fp.edch.number-of-mac-is-pdus", FT_UINT8, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_mac_is_pdu,
              { "Mac-is PDU",
                "fp.edch.mac-is-pdu", FT_BYTES, BASE_NONE, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_e_rnti,
              { "E-RNTI",
                "fp.edch.e-rnti", FT_UINT16, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },

            { &hf_fp_edch_macis_descriptors,
              { "MAC-is Descriptors",
                "fp.edch.mac-is.descriptors", FT_STRING, BASE_NONE, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_macis_lchid,
              { "LCH-ID",
                "fp.edch.mac-is.lchid", FT_UINT8, BASE_HEX, VALS(lchid_vals), 0xf0,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_macis_length,
              { "Length",
                "fp.edch.mac-is.length", FT_UINT16, BASE_DEC, 0, 0x0ffe,
                NULL, HFILL
              }
            },
            { &hf_fp_edch_macis_flag,
              { "Flag",
                "fp.edch.mac-is.lchid", FT_UINT8, BASE_HEX, 0, 0x01,
                "Indicates if another entry follows", HFILL
              }
            },
            { &hf_fp_frame_seq_nr,
              { "Frame Seq Nr",
                "fp.frame-seq-nr", FT_UINT8, BASE_DEC, 0, 0xf0,
                "Frame Sequence Number", HFILL
              }
            },
            { &hf_fp_hsdsch_pdu_block_header,
              { "PDU block header",
                "fp.hsdsch.pdu-block-header", FT_STRING, BASE_NONE, NULL, 0x0,
                "HS-DSCH type 2 PDU block header", HFILL
              }
            },
#if 0
            { &hf_fp_hsdsch_pdu_block,
              { "PDU block",
                "fp.hsdsch.pdu-block", FT_STRING, BASE_NONE, NULL, 0x0,
                "HS-DSCH type 2 PDU block data", HFILL
              }
            },
#endif
            { &hf_fp_flush,
              { "Flush",
                "fp.flush", FT_UINT8, BASE_DEC, 0, 0x04,
                "Whether all PDUs for this priority queue should be removed", HFILL
              }
            },
            { &hf_fp_fsn_drt_reset,
              { "FSN-DRT reset",
                "fp.fsn-drt-reset", FT_UINT8, BASE_DEC, 0, 0x02,
                "FSN/DRT Reset Flag", HFILL
              }
            },
            { &hf_fp_drt_indicator,
              { "DRT Indicator",
                "fp.drt-indicator", FT_UINT8, BASE_DEC, 0, 0x01,
                NULL, HFILL
              }
            },
            { &hf_fp_fach_indicator,
              { "FACH Indicator",
                "fp.fach-indicator", FT_UINT8, BASE_DEC, 0, 0x80,
                NULL, HFILL
              }
            },
            { &hf_fp_total_pdu_blocks,
              { "PDU Blocks",
                "fp.pdu_blocks", FT_UINT8, BASE_DEC, 0, 0xf8,
                "Total number of PDU blocks", HFILL
              }
            },
            { &hf_fp_drt,
              { "DelayRefTime",
                "fp.drt", FT_UINT16, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_hrnti,
              { "HRNTI",
                "fp.hrnti", FT_UINT16, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_measurement_result,
              { "RACH Measurement Result",
                "fp.rach-measurement-result", FT_UINT16, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_lchid,
              { "Logical Channel ID",
                "fp.lchid", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_pdu_length_in_block,
              { "PDU length in block",
                "fp.pdu-length-in-block", FT_UINT8, BASE_DEC, 0, 0x0,
                "Length of each PDU in this block in bytes", HFILL
              }
            },
            { &hf_fp_pdus_in_block,
              { "PDUs in block",
                "fp.no-pdus-in-block", FT_UINT8, BASE_DEC, 0, 0x0,
                "Number of PDUs in block", HFILL
              }
            },
            { &hf_fp_cmch_pi,
              { "CmCH-PI",
                "fp.cmch-pi", FT_UINT8, BASE_DEC, 0, 0x0f,
                "Common Transport Channel Priority Indicator", HFILL
              }
            },
            { &hf_fp_user_buffer_size,
              { "User buffer size",
                "fp.user-buffer-size", FT_UINT16, BASE_DEC, 0, 0x0,
                "User buffer size in octets", HFILL
              }
            },
            { &hf_fp_hsdsch_credits,
              { "HS-DSCH Credits",
                "fp.hsdsch-credits", FT_UINT16, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_max_macd_pdu_len,
              { "Max MAC-d PDU Length",
                "fp.hsdsch.max-macd-pdu-len", FT_UINT16, BASE_DEC, 0, 0xfff8,
                "Maximum MAC-d PDU Length in bits", HFILL
              }
            },
            { &hf_fp_hsdsch_max_macdc_pdu_len,
              { "Max MAC-d/c PDU Length",
                "fp.hsdsch.max-macdc-pdu-len", FT_UINT16, BASE_DEC, 0, 0x07ff,
                "Maximum MAC-d/c PDU Length in bits", HFILL
              }
            },
            { &hf_fp_hsdsch_interval,
              { "HS-DSCH Interval in milliseconds",
                "fp.hsdsch-interval", FT_UINT8, BASE_DEC, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_calculated_rate,
              { "Calculated rate allocation (bps)",
                "fp.hsdsch-calculated-rate", FT_UINT32, BASE_DEC, 0, 0x0,
                "Calculated rate RNC is allowed to send in bps", HFILL
              }
            },
            { &hf_fp_hsdsch_unlimited_rate,
              { "Unlimited rate",
                "fp.hsdsch-unlimited-rate", FT_NONE, BASE_NONE, 0, 0x0,
                "No restriction on rate at which date may be sent", HFILL
              }
            },
            { &hf_fp_hsdsch_repetition_period,
              { "HS-DSCH Repetition Period",
                "fp.hsdsch-repetition-period", FT_UINT8, BASE_DEC, 0, 0x0,
                "HS-DSCH Repetition Period in milliseconds", HFILL
              }
            },
            { &hf_fp_hsdsch_data_padding,
              { "Padding",
                "fp.hsdsch-data-padding", FT_UINT8, BASE_DEC, 0, 0xf0,
                "HS-DSCH Repetition Period in milliseconds", HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flags,
              { "New IEs flags",
                "fp.hsdsch.new-ie-flags", FT_STRING, BASE_NONE, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[0],
              { "DRT IE present",
                "fp.hsdsch.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x80,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[1],
              { "New IE present",
                "fp.hsdsch.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x40,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[2],
              { "New IE present",
                "fp.hsdsch.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x20,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[3],
              { "New IE present",
                "fp.hsdsch.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x10,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[4],
              { "New IE present",
                "fp.hsdsch.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x08,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[5],
              { "New IE present",
                "fp.hsdsch.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x04,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[6],
              { "HS-DSCH physical layer category present",
                "fp.hsdsch.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x02,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_new_ie_flag[7],
              { "Another new IE flags byte",
                "fp.hsdsch.new-ie-flags-byte", FT_UINT8, BASE_DEC, 0, 0x01,
                "Another new IE flagsbyte", HFILL
              }
            },
            { &hf_fp_hsdsch_drt,
              { "DRT",
                "fp.hsdsch.drt", FT_UINT8, BASE_DEC, 0, 0xf0,
                "Delay Reference Time", HFILL
              }
            },
            { &hf_fp_hsdsch_entity,
              { "HS-DSCH Entity",
                "fp.hsdsch.entity", FT_UINT8, BASE_DEC, VALS(hsdshc_mac_entity_vals), 0x0,
                "Type of MAC entity for this HS-DSCH channel", HFILL
              }
            },
            { &hf_fp_timing_advance,
              { "Timing advance",
                "fp.timing-advance", FT_UINT8, BASE_DEC, 0, 0x3f,
                "Timing advance in chips", HFILL
              }
            },
            { &hf_fp_num_of_pdu,
              { "Number of PDUs",
                "fp.hsdsch.num-of-pdu", FT_UINT8, BASE_DEC, 0, 0x0,
                "Number of PDUs in the payload", HFILL
              }
            },
            { &hf_fp_mac_d_pdu_len,
              { "MAC-d PDU Length",
                "fp.hsdsch.mac-d-pdu-len", FT_UINT16, BASE_DEC, 0, 0xfff8,
                "MAC-d PDU Length in bits", HFILL
              }
            },
            { &hf_fp_mac_d_pdu,
              { "MAC-d PDU",
                "fp.mac-d-pdu", FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_data,
              { "Data",
                "fp.data", FT_BYTES, BASE_NONE, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_crcis,
              { "CRCIs",
                "fp.crcis", FT_BYTES, BASE_NONE, NULL, 0x0,
                "CRC Indicators for uplink TBs", HFILL
              }
            },
            { &hf_fp_t1,
              { "T1",
                "fp.t1", FT_UINT24, BASE_DEC, NULL, 0x0,
                "RNC frame number indicating time it sends frame", HFILL
              }
            },
            { &hf_fp_t2,
              { "T2",
                "fp.t2", FT_UINT24, BASE_DEC, NULL, 0x0,
                "NodeB frame number indicating time it received DL Sync", HFILL
              }
            },
            { &hf_fp_t3,
              { "T3",
                "fp.t3", FT_UINT24, BASE_DEC, NULL, 0x0,
                "NodeB frame number indicating time it sends frame", HFILL
              }
            },
            { &hf_fp_ul_sir_target,
              { "UL_SIR_TARGET",
                "fp.ul-sir-target", FT_FLOAT, BASE_NONE, 0, 0x0,
                "Value (in dB) of the SIR target to be used by the UL inner loop power control", HFILL
              }
            },
            { &hf_fp_pusch_set_id,
              { "PUSCH Set Id",
                "fp.pusch-set-id", FT_UINT8, BASE_DEC, NULL, 0x0,
                "Identifies PUSCH Set from those configured in NodeB", HFILL
              }
            },
            { &hf_fp_activation_cfn,
              { "Activation CFN",
                "fp.activation-cfn", FT_UINT8, BASE_DEC, NULL, 0x0,
                "Activation Connection Frame Number", HFILL
              }
            },
            { &hf_fp_duration,
              { "Duration (ms)",
                "fp.pusch-set-id", FT_UINT8, BASE_DEC, NULL, 0x0,
                "Duration of the activation period of the PUSCH Set", HFILL
              }
            },
            { &hf_fp_power_offset,
              { "Power offset",
                "fp.power-offset", FT_FLOAT, BASE_NONE, NULL, 0x0,
                "Power offset (in dB)", HFILL
              }
            },
            { &hf_fp_code_number,
              { "Code number",
                "fp.code-number", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_spreading_factor,
              { "Spreading factor",
                "fp.spreading-factor", FT_UINT8, BASE_DEC, VALS(spreading_factor_vals), 0xf0,
                NULL, HFILL
              }
            },
            { &hf_fp_mc_info,
              { "MC info",
                "fp.mc-info", FT_UINT8, BASE_DEC, NULL, 0x0e,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_new_ie_flags,
              { "New IEs flags",
                "fp.rach.new-ie-flags", FT_STRING, BASE_NONE, 0, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_new_ie_flag_unused[0],
              { "New IE present",
                "fp.rach.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x80,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_new_ie_flag_unused[1],
              { "New IE present",
                "fp.rach.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x40,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_new_ie_flag_unused[2],
              { "New IE present",
                "fp.rach.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x20,
                "New IE present (unused)", HFILL
              }
            },
            { &hf_fp_rach_new_ie_flag_unused[3],
              { "New IE present",
                "fp.rach.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x10,
                "New IE present (unused)", HFILL
              }
            },
            { &hf_fp_rach_new_ie_flag_unused[4],
              { "New IE present",
                "fp.rach.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x08,
                "New IE present (unused)", HFILL
              }
            },
            { &hf_fp_rach_new_ie_flag_unused[5],
              { "New IE present",
                "fp.rach.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x04,
                "New IE present (unused)", HFILL
              }
            },
            { &hf_fp_rach_new_ie_flag_unused[6],
              { "New IE present",
                "fp.rach.new-ie-flag", FT_UINT8, BASE_DEC, 0, 0x02,
                "New IE present (unused)", HFILL
              }
            },
            { &hf_fp_rach_cell_portion_id_present,
              { "Cell portion ID present",
                "fp.rach.cell-portion-id-present", FT_UINT8, BASE_DEC, 0, 0x01,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_angle_of_arrival_present,
              { "Angle of arrival present",
                "fp.rach.angle-of-arrival-present", FT_UINT8, BASE_DEC, 0, 0x01,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_ext_propagation_delay_present,
              { "Ext Propagation Delay Present",
                "fp.rach.ext-propagation-delay-present", FT_UINT8, BASE_DEC, 0, 0x02,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_ext_rx_sync_ul_timing_deviation_present,
              { "Ext Received Sync UL Timing Deviation present",
                "fp.rach.ext-rx-sync-ul-timing-deviation-present", FT_UINT8, BASE_DEC, 0, 0x02,
                NULL, HFILL
              }
            },
            { &hf_fp_rach_ext_rx_timing_deviation_present,
              { "Ext Rx Timing Deviation present",
                "fp.rach.ext-rx-timing-deviation-present", FT_UINT8, BASE_DEC, 0, 0x01,
                NULL, HFILL
              }
            },
            { &hf_fp_cell_portion_id,
              { "Cell Portion ID",
                "fp.cell-portion-id", FT_UINT8, BASE_DEC, NULL, 0x3f,
                NULL, HFILL
              }
            },
            { &hf_fp_ext_propagation_delay,
              { "Ext Propagation Delay",
                "fp.ext-propagation-delay", FT_UINT16, BASE_DEC, NULL, 0x03ff,
                NULL, HFILL
              }
            },
            { &hf_fp_angle_of_arrival,
              { "Angle of Arrival",
                "fp.angle-of-arrival", FT_UINT16, BASE_DEC, NULL, 0x03ff,
                NULL, HFILL
              }
            },
            { &hf_fp_ext_received_sync_ul_timing_deviation,
              { "Ext Received SYNC UL Timing Deviation",
                "fp.ext-received-sync-ul-timing-deviation", FT_UINT16, BASE_DEC, NULL, 0x1fff,
                NULL, HFILL
              }
            },


            { &hf_fp_radio_interface_parameter_update_flag[0],
              { "CFN valid",
                "fp.radio-interface-param.cfn-valid", FT_UINT16, BASE_DEC, 0, 0x0001,
                NULL, HFILL
              }
            },
            { &hf_fp_radio_interface_parameter_update_flag[1],
              { "TPC PO valid",
                "fp.radio-interface-param.tpc-po-valid", FT_UINT16, BASE_DEC, 0, 0x0002,
                NULL, HFILL
              }
            },
            { &hf_fp_radio_interface_parameter_update_flag[2],
              { "DPC mode valid",
                "fp.radio-interface-param.dpc-mode-valid", FT_UINT16, BASE_DEC, 0, 0x0004,
                NULL, HFILL
              }
            },
            { &hf_fp_radio_interface_parameter_update_flag[3],
              { "RL sets indicator valid",
                "fp.radio-interface_param.rl-sets-indicator-valid", FT_UINT16, BASE_DEC, 0, 0x0020,
                NULL, HFILL
              }
            },
            { &hf_fp_radio_interface_parameter_update_flag[4],
              { "MAX_UE_TX_POW valid",
                "fp.radio-interface-param.max-ue-tx-pow-valid", FT_UINT16, BASE_DEC, 0, 0x0040,
                "MAX UE TX POW valid", HFILL
              }
            },
            { &hf_fp_dpc_mode,
              { "DPC Mode",
                "fp.dpc-mode", FT_UINT8, BASE_DEC, NULL, 0x20,
                "DPC Mode to be applied in the uplink", HFILL
              }
            },
            { &hf_fp_tpc_po,
              { "TPC PO",
                "fp.tpc-po", FT_UINT8, BASE_DEC, NULL, 0x1f,
                NULL, HFILL
              }
            },
            { &hf_fp_multiple_rl_set_indicator,
              { "Multiple RL sets indicator",
                "fp.multiple-rl-sets-indicator", FT_UINT8, BASE_DEC, NULL, 0x80,
                NULL, HFILL
              }
            },
            { &hf_fp_max_ue_tx_pow,
              { "MAX_UE_TX_POW",
                "fp.max-ue-tx-pow", FT_INT8, BASE_DEC, NULL, 0x0,
                "Max UE TX POW (dBm)", HFILL
              }
            },
            { &hf_fp_congestion_status,
              { "Congestion Status",
                "fp.congestion-status", FT_UINT8, BASE_DEC, VALS(congestion_status_vals), 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_e_rucch_present,
              { "E-RUCCH Present",
                "fp.erucch-present", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_extended_bits_present,
              { "Extended Bits Present",
                "fp.extended-bits-present", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_extended_bits,
              { "Extended Bits",
                "fp.extended-bits", FT_UINT8, BASE_HEX, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_spare_extension,
              { "Spare Extension",
                "fp.spare-extension", FT_NONE, BASE_NONE, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_ul_setup_frame,
              { "UL setup frame",
                "fp.ul.setup_frame", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_dl_setup_frame,
              { "DL setup frame",
                "fp.dl.setup_frame", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
                NULL, HFILL
              }
            },
            { &hf_fp_hsdsch_physical_layer_category,
              { "HS-DSCH physical layer category",
                "fp.hsdsch.physical_layer_category", FT_UINT8, BASE_DEC, NULL, 0x0,
                NULL, HFILL
              }
            }
        };


    static gint *ett[] =
    {
        &ett_fp,
        &ett_fp_data,
        &ett_fp_crcis,
        &ett_fp_ddi_config,
        &ett_fp_edch_subframe_header,
        &ett_fp_edch_subframe,
        &ett_fp_edch_maces,
        &ett_fp_edch_macis_descriptors,
        &ett_fp_hsdsch_new_ie_flags,
        &ett_fp_rach_new_ie_flags,
        &ett_fp_hsdsch_pdu_block_header,
        &ett_fp_release
    };

    static ei_register_info ei[] = {
        { &ei_fp_bad_header_checksum, { "fp.header.bad_checksum.", PI_CHECKSUM, PI_WARN, "Bad header checksum.", EXPFILL }},
        { &ei_fp_crci_no_subdissector, { "fp.crci.no_subdissector", PI_UNDECODED, PI_NOTE, "Not sent to subdissectors as CRCI is set", EXPFILL }},
        { &ei_fp_crci_error_bit_set_for_tb, { "fp.crci.error_bit_set_for_tb", PI_CHECKSUM, PI_WARN, "CRCI error bit set for TB", EXPFILL }},
        { &ei_fp_spare_extension, { "fp.spare-extension.expert", PI_UNDECODED, PI_WARN, "Spare Extension present (%u bytes)", EXPFILL }},
        { &ei_fp_bad_payload_checksum, { "fp.payload-crc.bad.", PI_CHECKSUM, PI_WARN, "Bad payload checksum.", EXPFILL }},
        { &ei_fp_stop_hsdpa_transmission, { "fp.stop_hsdpa_transmission", PI_RESPONSE_CODE, PI_NOTE, "Stop HSDPA transmission", EXPFILL }},
        { &ei_fp_timing_adjustmentment_reported, { "fp.timing_adjustmentment_reported", PI_SEQUENCE, PI_WARN, "Timing adjustmentment reported (%f ms)", EXPFILL }},
        { &ei_fp_expecting_tdd, { "fp.expecting_tdd", PI_MALFORMED, PI_NOTE, "Error: expecting TDD-384 or TDD-768", EXPFILL }},
        { &ei_fp_ddi_not_defined, { "fp.ddi_not_defined", PI_MALFORMED, PI_ERROR, "DDI %u not defined for this UE!", EXPFILL }},
        { &ei_fp_unable_to_locate_ddi_entry, { "fp.unable_to_locate_ddi_entry", PI_UNDECODED, PI_ERROR, "Unable to locate DDI entry.", EXPFILL }},
        { &ei_fp_mac_is_sdus_miscount, { "fp.mac_is_sdus.miscount", PI_MALFORMED, PI_ERROR, "Found too many (%u) MAC-is SDUs - header said there were %u", EXPFILL }},
        { &ei_fp_e_rnti_t2_edch_frames, { "fp.e_rnti.t2_edch_frames", PI_MALFORMED, PI_ERROR, "E-RNTI not supposed to appear for T2 EDCH frames", EXPFILL }},
        { &ei_fp_e_rnti_first_entry, { "fp.e_rnti.first_entry", PI_MALFORMED, PI_ERROR, "E-RNTI must be first entry among descriptors", EXPFILL }},
        { &ei_fp_maybe_srb, { "fp.maybe_srb", PI_PROTOCOL, PI_NOTE, "Found MACd-Flow = 0 and not MUX detected. (This might be SRB)", EXPFILL }},
        { &ei_fp_transport_channel_type_unknown, { "fp.transport_channel_type.unknown", PI_UNDECODED, PI_WARN, "Unknown transport channel type", EXPFILL }},
        { &ei_fp_hsdsch_entity_not_specified, { "fp.hsdsch_entity_not_specified", PI_MALFORMED, PI_ERROR, "HSDSCH Entity not specified", EXPFILL }},
        { &ei_fp_hsdsch_common_experimental_support, { "fp.hsdsch_common.experimental_support", PI_DEBUG, PI_WARN, "HSDSCH COMMON - Experimental support!", EXPFILL }},
        { &ei_fp_hsdsch_common_t3_not_implemented, { "fp.hsdsch_common_t3.not_implemented", PI_DEBUG, PI_ERROR, "HSDSCH COMMON T3 - Not implemeneted!", EXPFILL }},
        { &ei_fp_channel_type_unknown, { "fp.channel_type.unknown", PI_MALFORMED, PI_ERROR, "Unknown channel type", EXPFILL }},
        { &ei_fp_no_per_frame_info, { "fp.no_per_frame_info", PI_UNDECODED, PI_ERROR, "Can't dissect FP frame because no per-frame info was attached!", EXPFILL }},
    };

    module_t *fp_module;
    expert_module_t *expert_fp;

#ifdef UMTS_FP_USE_UAT
    /* Define a UAT to set channel configuration data */

  static const value_string umts_fp_proto_type_vals[] = {
    { UMTS_FP_IPV4, "IPv4" },
    { UMTS_FP_IPV6, "IPv6" },
    { 0x00, NULL }
  };
  static const value_string umts_fp_uat_channel_type_vals[] = {
    { CHANNEL_RACH_FDD, "RACH FDD" },
    { 0x00, NULL }
  };
  static const value_string umts_fp_uat_interface_type_vals[] = {
    { IuB_Interface, "IuB Interface" },
    { 0x00, NULL }
  };
  static const value_string umts_fp_uat_division_type_vals[] = {
    { Division_FDD, "Division FDD" },
    { 0x00, NULL }
  };

  static const value_string umts_fp_uat_rlc_mode_vals[] = {
    { FP_RLC_TM, "FP RLC TM" },
    { 0x00, NULL }
  };

  static uat_field_t umts_fp_uat_flds[] = {
      UAT_FLD_VS(uat_umts_fp_ep_and_ch_records, protocol, "IP address type", umts_fp_proto_type_vals, "IPv4 or IPv6"),
      UAT_FLD_CSTRING(uat_umts_fp_ep_and_ch_records, srcIP, "RNC IP Address", "Source Address"),
      UAT_FLD_DEC(uat_umts_fp_ep_and_ch_records, src_port, "RNC port for this channel", "Source port"),
      UAT_FLD_CSTRING(uat_umts_fp_ep_and_ch_records, dstIP, "NodeB IP Address", "Destination Address"),
      UAT_FLD_DEC(uat_umts_fp_ep_and_ch_records, dst_port, "NodeB port for this channel", "Destination port"),
      UAT_FLD_VS(uat_umts_fp_ep_and_ch_records, interface_type, "Interface type", umts_fp_uat_interface_type_vals, "Interface type used"),
      UAT_FLD_VS(uat_umts_fp_ep_and_ch_records, division, "division", umts_fp_uat_division_type_vals, "Division type used"),
      UAT_FLD_VS(uat_umts_fp_ep_and_ch_records, channel_type, "Channel type", umts_fp_uat_channel_type_vals, "Channel type used"),
      UAT_FLD_VS(uat_umts_fp_ep_and_ch_records, rlc_mode, "RLC mode", umts_fp_uat_rlc_mode_vals, "RLC mode used"),
      UAT_END_FIELDS
    };
#endif /* UMTS_FP_USE_UAT */
    /* Register protocol. */
    proto_fp = proto_register_protocol("FP", "FP", "fp");
    proto_register_field_array(proto_fp, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
    expert_fp = expert_register_protocol(proto_fp);
    expert_register_field_array(expert_fp, ei, array_length(ei));

    /* Allow other dissectors to find this one by name. */
    fp_handle = register_dissector("fp", dissect_fp, proto_fp);

    /* Preferences */
    fp_module = prefs_register_protocol(proto_fp, NULL);

    /* Determines whether release information should be displayed */
    prefs_register_bool_preference(fp_module, "show_release_info",
                                   "Show reported release info",
                                   "Show reported release info",
                                   &preferences_show_release_info);

    /* Determines whether MAC dissector should be called for payloads */
    prefs_register_bool_preference(fp_module, "call_mac",
                                   "Call MAC dissector for payloads",
                                   "Call MAC dissector for payloads",
                                   &preferences_call_mac_dissectors);
     /* Determines whether or not to validate FP payload checksums */
    prefs_register_bool_preference(fp_module, "payload_checksum",
                                    "Validate FP payload checksums",
                                    "Validate FP payload checksums",
                                    &preferences_payload_checksum);
     /* Determines whether or not to validate FP header checksums */
    prefs_register_bool_preference(fp_module, "header_checksum",
                                    "Validate FP header checksums",
                                    "Validate FP header checksums",
                                    &preferences_header_checksum);
     /* Determines whether or not to validate FP header checksums */
    prefs_register_obsolete_preference(fp_module, "udp_heur");
#ifdef UMTS_FP_USE_UAT

  umts_fp_uat = uat_new("Endpoint and Channel Configuration",
            sizeof(uat_umts_fp_ep_and_ch_record_t),   /* record size */
            "umts_fp_ep_and_channel_cnf",     /* filename */
            TRUE,                             /* from_profile */
            &uat_umts_fp_ep_and_ch_records,   /* data_ptr */
            &num_umts_fp_ep_and_ch_items,     /* numitems_ptr */
            UAT_AFFECTS_DISSECTION,           /* affects dissection of packets, but not set of named fields */
            NULL,                             /* help */
            uat_umts_fp_record_copy_cb,       /* copy callback */
            NULL,                             /* update callback */
            uat_umts_fp_record_free_cb,       /* free callback */
            NULL,                             /* post update callback */
            umts_fp_uat_flds);                /* UAT field definitions */

  prefs_register_uat_preference(fp_module,
                                "epandchannelconfigurationtable",
                                "Endpoints and Radio Channels configuration",
                                "Preconfigured endpoint and Channels data",
                                umts_fp_uat);

  register_init_routine(&umts_fp_init_protocol);
#endif

}
