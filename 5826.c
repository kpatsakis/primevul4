void isor_check_producer_ref_time(ISOMReader *read)
{
	GF_ISOTrackID trackID;
	u64 ntp;
	u64 timestamp;

	if (gf_sys_is_test_mode()) {
		return;
	}

	if (gf_isom_get_last_producer_time_box(read->mov, &trackID, &ntp, &timestamp, GF_TRUE)) {
#if !defined(_WIN32_WCE) && !defined(GPAC_DISABLE_LOG)

		if (gf_log_tool_level_on(GF_LOG_DASH, GF_LOG_DEBUG)) {
			time_t secs;
			struct tm t;

			s32 diff = gf_net_get_ntp_diff_ms(ntp);

			secs = (ntp>>32) - GF_NTP_SEC_1900_TO_1970;
			t = *gf_gmtime(&secs);

			GF_LOG(GF_LOG_DEBUG, GF_LOG_DASH, ("[IsoMedia] TrackID %d: Timestamp "LLU" matches sender NTP time %d-%02d-%02dT%02d:%02d:%02dZ - NTP clock diff (local - remote): %d ms\n", trackID, timestamp, 1900+t.tm_year, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, diff));
		}
#endif
		read->last_sender_ntp = ntp;
		read->cts_for_last_sender_ntp = timestamp;
		read->ntp_at_last_sender_ntp = gf_net_get_ntp_ts();
	}
}