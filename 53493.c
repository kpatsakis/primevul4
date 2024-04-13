DOS_FILE *get_owner(DOS_FS * fs, uint32_t cluster)
{
    if (fs->cluster_owner == NULL)
	return NULL;
    else
	return fs->cluster_owner[cluster];
}
