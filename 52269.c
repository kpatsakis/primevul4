free_SubStreamsInfo(struct _7z_substream_info *ss)
{
	free(ss->unpackSizes);
	free(ss->digestsDefined);
	free(ss->digests);
}
