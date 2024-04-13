loff_t cluster_start(DOS_FS * fs, uint32_t cluster)
{
    return fs->data_start + ((loff_t) cluster -
			     2) * (uint64_t)fs->cluster_size;
}
