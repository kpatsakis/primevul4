free_PackInfo(struct _7z_pack_info *pi)
{
	free(pi->sizes);
	free(pi->positions);
	free_Digest(&(pi->digest));
}
