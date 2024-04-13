free_StreamsInfo(struct _7z_stream_info *si)
{
	free_PackInfo(&(si->pi));
	free_CodersInfo(&(si->ci));
	free_SubStreamsInfo(&(si->ss));
}
