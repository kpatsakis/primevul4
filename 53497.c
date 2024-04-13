void set_owner(DOS_FS * fs, uint32_t cluster, DOS_FILE * owner)
{
    if (fs->cluster_owner == NULL)
	die("Internal error: attempt to set owner in non-existent table");

    if (owner && fs->cluster_owner[cluster]
	&& (fs->cluster_owner[cluster] != owner))
	die("Internal error: attempt to change file owner");
    fs->cluster_owner[cluster] = owner;
}
