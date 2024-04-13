print_radiotap_field(netdissect_options *ndo,
                     struct cpack_state *s, uint32_t bit, uint8_t *flagsp,
                     uint32_t presentflags)
{
	u_int i;
	int rc;

	switch (bit) {

	case IEEE80211_RADIOTAP_TSFT: {
		uint64_t tsft;

		rc = cpack_uint64(s, &tsft);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%" PRIu64 "us tsft ", tsft));
		break;
		}

	case IEEE80211_RADIOTAP_FLAGS: {
		uint8_t flagsval;

		rc = cpack_uint8(s, &flagsval);
		if (rc != 0)
			goto trunc;
		*flagsp = flagsval;
		if (flagsval & IEEE80211_RADIOTAP_F_CFP)
			ND_PRINT((ndo, "cfp "));
		if (flagsval & IEEE80211_RADIOTAP_F_SHORTPRE)
			ND_PRINT((ndo, "short preamble "));
		if (flagsval & IEEE80211_RADIOTAP_F_WEP)
			ND_PRINT((ndo, "wep "));
		if (flagsval & IEEE80211_RADIOTAP_F_FRAG)
			ND_PRINT((ndo, "fragmented "));
		if (flagsval & IEEE80211_RADIOTAP_F_BADFCS)
			ND_PRINT((ndo, "bad-fcs "));
		break;
		}

	case IEEE80211_RADIOTAP_RATE: {
		uint8_t rate;

		rc = cpack_uint8(s, &rate);
		if (rc != 0)
			goto trunc;
		/*
		 * XXX On FreeBSD rate & 0x80 means we have an MCS. On
		 * Linux and AirPcap it does not.  (What about
		 * Mac OS X, NetBSD, OpenBSD, and DragonFly BSD?)
		 *
		 * This is an issue either for proprietary extensions
		 * to 11a or 11g, which do exist, or for 11n
		 * implementations that stuff a rate value into
		 * this field, which also appear to exist.
		 *
		 * We currently handle that by assuming that
		 * if the 0x80 bit is set *and* the remaining
		 * bits have a value between 0 and 15 it's
		 * an MCS value, otherwise it's a rate.  If
		 * there are cases where systems that use
		 * "0x80 + MCS index" for MCS indices > 15,
		 * or stuff a rate value here between 64 and
		 * 71.5 Mb/s in here, we'll need a preference
		 * setting.  Such rates do exist, e.g. 11n
		 * MCS 7 at 20 MHz with a long guard interval.
		 */
		if (rate >= 0x80 && rate <= 0x8f) {
			/*
			 * XXX - we don't know the channel width
			 * or guard interval length, so we can't
			 * convert this to a data rate.
			 *
			 * If you want us to show a data rate,
			 * use the MCS field, not the Rate field;
			 * the MCS field includes not only the
			 * MCS index, it also includes bandwidth
			 * and guard interval information.
			 *
			 * XXX - can we get the channel width
			 * from XChannel and the guard interval
			 * information from Flags, at least on
			 * FreeBSD?
			 */
			ND_PRINT((ndo, "MCS %u ", rate & 0x7f));
		} else
			ND_PRINT((ndo, "%2.1f Mb/s ", .5 * rate));
		break;
		}

	case IEEE80211_RADIOTAP_CHANNEL: {
		uint16_t frequency;
		uint16_t flags;

		rc = cpack_uint16(s, &frequency);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint16(s, &flags);
		if (rc != 0)
			goto trunc;
		/*
		 * If CHANNEL and XCHANNEL are both present, skip
		 * CHANNEL.
		 */
		if (presentflags & (1 << IEEE80211_RADIOTAP_XCHANNEL))
			break;
		print_chaninfo(ndo, frequency, flags, presentflags);
		break;
		}

	case IEEE80211_RADIOTAP_FHSS: {
		uint8_t hopset;
		uint8_t hoppat;

		rc = cpack_uint8(s, &hopset);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &hoppat);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "fhset %d fhpat %d ", hopset, hoppat));
		break;
		}

	case IEEE80211_RADIOTAP_DBM_ANTSIGNAL: {
		int8_t dbm_antsignal;

		rc = cpack_int8(s, &dbm_antsignal);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%ddBm signal ", dbm_antsignal));
		break;
		}

	case IEEE80211_RADIOTAP_DBM_ANTNOISE: {
		int8_t dbm_antnoise;

		rc = cpack_int8(s, &dbm_antnoise);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%ddBm noise ", dbm_antnoise));
		break;
		}

	case IEEE80211_RADIOTAP_LOCK_QUALITY: {
		uint16_t lock_quality;

		rc = cpack_uint16(s, &lock_quality);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%u sq ", lock_quality));
		break;
		}

	case IEEE80211_RADIOTAP_TX_ATTENUATION: {
		uint16_t tx_attenuation;

		rc = cpack_uint16(s, &tx_attenuation);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%d tx power ", -(int)tx_attenuation));
		break;
		}

	case IEEE80211_RADIOTAP_DB_TX_ATTENUATION: {
		uint8_t db_tx_attenuation;

		rc = cpack_uint8(s, &db_tx_attenuation);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%ddB tx attenuation ", -(int)db_tx_attenuation));
		break;
		}

	case IEEE80211_RADIOTAP_DBM_TX_POWER: {
		int8_t dbm_tx_power;

		rc = cpack_int8(s, &dbm_tx_power);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%ddBm tx power ", dbm_tx_power));
		break;
		}

	case IEEE80211_RADIOTAP_ANTENNA: {
		uint8_t antenna;

		rc = cpack_uint8(s, &antenna);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "antenna %u ", antenna));
		break;
		}

	case IEEE80211_RADIOTAP_DB_ANTSIGNAL: {
		uint8_t db_antsignal;

		rc = cpack_uint8(s, &db_antsignal);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%ddB signal ", db_antsignal));
		break;
		}

	case IEEE80211_RADIOTAP_DB_ANTNOISE: {
		uint8_t db_antnoise;

		rc = cpack_uint8(s, &db_antnoise);
		if (rc != 0)
			goto trunc;
		ND_PRINT((ndo, "%ddB noise ", db_antnoise));
		break;
		}

	case IEEE80211_RADIOTAP_RX_FLAGS: {
		uint16_t rx_flags;

		rc = cpack_uint16(s, &rx_flags);
		if (rc != 0)
			goto trunc;
		/* Do nothing for now */
		break;
		}

	case IEEE80211_RADIOTAP_XCHANNEL: {
		uint32_t flags;
		uint16_t frequency;
		uint8_t channel;
		uint8_t maxpower;

		rc = cpack_uint32(s, &flags);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint16(s, &frequency);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &channel);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &maxpower);
		if (rc != 0)
			goto trunc;
		print_chaninfo(ndo, frequency, flags, presentflags);
		break;
		}

	case IEEE80211_RADIOTAP_MCS: {
		uint8_t known;
		uint8_t flags;
		uint8_t mcs_index;
		static const char *ht_bandwidth[4] = {
			"20 MHz",
			"40 MHz",
			"20 MHz (L)",
			"20 MHz (U)"
		};
		float htrate;

		rc = cpack_uint8(s, &known);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &flags);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &mcs_index);
		if (rc != 0)
			goto trunc;
		if (known & IEEE80211_RADIOTAP_MCS_MCS_INDEX_KNOWN) {
			/*
			 * We know the MCS index.
			 */
			if (mcs_index <= MAX_MCS_INDEX) {
				/*
				 * And it's in-range.
				 */
				if (known & (IEEE80211_RADIOTAP_MCS_BANDWIDTH_KNOWN|IEEE80211_RADIOTAP_MCS_GUARD_INTERVAL_KNOWN)) {
					/*
					 * And we know both the bandwidth and
					 * the guard interval, so we can look
					 * up the rate.
					 */
					htrate =
						ieee80211_float_htrates \
							[mcs_index] \
							[((flags & IEEE80211_RADIOTAP_MCS_BANDWIDTH_MASK) == IEEE80211_RADIOTAP_MCS_BANDWIDTH_40 ? 1 : 0)] \
							[((flags & IEEE80211_RADIOTAP_MCS_SHORT_GI) ? 1 : 0)];
				} else {
					/*
					 * We don't know both the bandwidth
					 * and the guard interval, so we can
					 * only report the MCS index.
					 */
					htrate = 0.0;
				}
			} else {
				/*
				 * The MCS value is out of range.
				 */
				htrate = 0.0;
			}
			if (htrate != 0.0) {
				/*
				 * We have the rate.
				 * Print it.
				 */
				ND_PRINT((ndo, "%.1f Mb/s MCS %u ", htrate, mcs_index));
			} else {
				/*
				 * We at least have the MCS index.
				 * Print it.
				 */
				ND_PRINT((ndo, "MCS %u ", mcs_index));
			}
		}
		if (known & IEEE80211_RADIOTAP_MCS_BANDWIDTH_KNOWN) {
			ND_PRINT((ndo, "%s ",
				ht_bandwidth[flags & IEEE80211_RADIOTAP_MCS_BANDWIDTH_MASK]));
		}
		if (known & IEEE80211_RADIOTAP_MCS_GUARD_INTERVAL_KNOWN) {
			ND_PRINT((ndo, "%s GI ",
				(flags & IEEE80211_RADIOTAP_MCS_SHORT_GI) ?
				"short" : "long"));
		}
		if (known & IEEE80211_RADIOTAP_MCS_HT_FORMAT_KNOWN) {
			ND_PRINT((ndo, "%s ",
				(flags & IEEE80211_RADIOTAP_MCS_HT_GREENFIELD) ?
				"greenfield" : "mixed"));
		}
		if (known & IEEE80211_RADIOTAP_MCS_FEC_TYPE_KNOWN) {
			ND_PRINT((ndo, "%s FEC ",
				(flags & IEEE80211_RADIOTAP_MCS_FEC_LDPC) ?
				"LDPC" : "BCC"));
		}
		if (known & IEEE80211_RADIOTAP_MCS_STBC_KNOWN) {
			ND_PRINT((ndo, "RX-STBC%u ",
				(flags & IEEE80211_RADIOTAP_MCS_STBC_MASK) >> IEEE80211_RADIOTAP_MCS_STBC_SHIFT));
		}
		break;
		}

	case IEEE80211_RADIOTAP_AMPDU_STATUS: {
		uint32_t reference_num;
		uint16_t flags;
		uint8_t delim_crc;
		uint8_t reserved;

		rc = cpack_uint32(s, &reference_num);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint16(s, &flags);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &delim_crc);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &reserved);
		if (rc != 0)
			goto trunc;
		/* Do nothing for now */
		break;
		}

	case IEEE80211_RADIOTAP_VHT: {
		uint16_t known;
		uint8_t flags;
		uint8_t bandwidth;
		uint8_t mcs_nss[4];
		uint8_t coding;
		uint8_t group_id;
		uint16_t partial_aid;
		static const char *vht_bandwidth[32] = {
			"20 MHz",
			"40 MHz",
			"20 MHz (L)",
			"20 MHz (U)",
			"80 MHz",
			"80 MHz (L)",
			"80 MHz (U)",
			"80 MHz (LL)",
			"80 MHz (LU)",
			"80 MHz (UL)",
			"80 MHz (UU)",
			"160 MHz",
			"160 MHz (L)",
			"160 MHz (U)",
			"160 MHz (LL)",
			"160 MHz (LU)",
			"160 MHz (UL)",
			"160 MHz (UU)",
			"160 MHz (LLL)",
			"160 MHz (LLU)",
			"160 MHz (LUL)",
			"160 MHz (UUU)",
			"160 MHz (ULL)",
			"160 MHz (ULU)",
			"160 MHz (UUL)",
			"160 MHz (UUU)",
			"unknown (26)",
			"unknown (27)",
			"unknown (28)",
			"unknown (29)",
			"unknown (30)",
			"unknown (31)"
		};

		rc = cpack_uint16(s, &known);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &flags);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &bandwidth);
		if (rc != 0)
			goto trunc;
		for (i = 0; i < 4; i++) {
			rc = cpack_uint8(s, &mcs_nss[i]);
			if (rc != 0)
				goto trunc;
		}
		rc = cpack_uint8(s, &coding);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint8(s, &group_id);
		if (rc != 0)
			goto trunc;
		rc = cpack_uint16(s, &partial_aid);
		if (rc != 0)
			goto trunc;
		for (i = 0; i < 4; i++) {
			u_int nss, mcs;
			nss = mcs_nss[i] & IEEE80211_RADIOTAP_VHT_NSS_MASK;
			mcs = (mcs_nss[i] & IEEE80211_RADIOTAP_VHT_MCS_MASK) >> IEEE80211_RADIOTAP_VHT_MCS_SHIFT;

			if (nss == 0)
				continue;

			ND_PRINT((ndo, "User %u MCS %u ", i, mcs));
			ND_PRINT((ndo, "%s FEC ",
				(coding & (IEEE80211_RADIOTAP_CODING_LDPC_USERn << i)) ?
				"LDPC" : "BCC"));
		}
		if (known & IEEE80211_RADIOTAP_VHT_BANDWIDTH_KNOWN) {
			ND_PRINT((ndo, "%s ",
				vht_bandwidth[bandwidth & IEEE80211_RADIOTAP_VHT_BANDWIDTH_MASK]));
		}
		if (known & IEEE80211_RADIOTAP_VHT_GUARD_INTERVAL_KNOWN) {
			ND_PRINT((ndo, "%s GI ",
				(flags & IEEE80211_RADIOTAP_VHT_SHORT_GI) ?
				"short" : "long"));
		}
		break;
		}

	default:
		/* this bit indicates a field whose
		 * size we do not know, so we cannot
		 * proceed.  Just print the bit number.
		 */
		ND_PRINT((ndo, "[bit %u] ", bit));
		return -1;
	}

	return 0;

trunc:
	ND_PRINT((ndo, "%s", tstr));
	return rc;
}
