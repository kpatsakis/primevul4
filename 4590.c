u64 gf_isom_get_mp4time()
{
	u32 calctime, msec;
	u64 ret;
	gf_utc_time_since_1970(&calctime, &msec);
	calctime += GF_ISOM_MAC_TIME_OFFSET;
	ret = calctime;
	return ret;
}