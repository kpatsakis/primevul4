static const char *isoffin_probe_data(const u8 *data, u32 size, GF_FilterProbeScore *score)
{
	if (gf_isom_probe_data(data, size)) {
		*score = GF_FPROBE_SUPPORTED;
		return "video/mp4";
	}
	return NULL;
}