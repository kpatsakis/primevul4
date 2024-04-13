static s32 brcmf_config_wowl_pattern(struct brcmf_if *ifp, u8 cmd[4],
				     u8 *pattern, u32 patternsize, u8 *mask,
				     u32 packet_offset)
{
	struct brcmf_fil_wowl_pattern_le *filter;
	u32 masksize;
	u32 patternoffset;
	u8 *buf;
	u32 bufsize;
	s32 ret;

	masksize = (patternsize + 7) / 8;
	patternoffset = sizeof(*filter) - sizeof(filter->cmd) + masksize;

	bufsize = sizeof(*filter) + patternsize + masksize;
	buf = kzalloc(bufsize, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	filter = (struct brcmf_fil_wowl_pattern_le *)buf;

	memcpy(filter->cmd, cmd, 4);
	filter->masksize = cpu_to_le32(masksize);
	filter->offset = cpu_to_le32(packet_offset);
	filter->patternoffset = cpu_to_le32(patternoffset);
	filter->patternsize = cpu_to_le32(patternsize);
	filter->type = cpu_to_le32(BRCMF_WOWL_PATTERN_TYPE_BITMAP);

	if ((mask) && (masksize))
		memcpy(buf + sizeof(*filter), mask, masksize);
	if ((pattern) && (patternsize))
		memcpy(buf + sizeof(*filter) + masksize, pattern, patternsize);

	ret = brcmf_fil_iovar_data_set(ifp, "wowl_pattern", buf, bufsize);

	kfree(buf);
	return ret;
}
