static void nfs_writeback_release_common(void *calldata)
{
	struct nfs_write_data	*data = calldata;
	struct nfs_pgio_header *hdr = data->header;
	int status = data->task.tk_status;

	if ((status >= 0) && nfs_write_need_commit(data)) {
		spin_lock(&hdr->lock);
		if (test_bit(NFS_IOHDR_NEED_RESCHED, &hdr->flags))
			; /* Do nothing */
		else if (!test_and_set_bit(NFS_IOHDR_NEED_COMMIT, &hdr->flags))
			memcpy(hdr->verf, &data->verf, sizeof(*hdr->verf));
		else if (memcmp(hdr->verf, &data->verf, sizeof(*hdr->verf)))
			set_bit(NFS_IOHDR_NEED_RESCHED, &hdr->flags);
		spin_unlock(&hdr->lock);
	}
	nfs_writedata_release(data);
}
