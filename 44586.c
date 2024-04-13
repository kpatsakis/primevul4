static struct nfs_write_data *nfs_writedata_alloc(struct nfs_pgio_header *hdr,
						  unsigned int pagecount)
{
	struct nfs_write_data *data, *prealloc;

	prealloc = &container_of(hdr, struct nfs_write_header, header)->rpc_data;
	if (prealloc->header == NULL)
		data = prealloc;
	else
		data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		goto out;

	if (nfs_pgarray_set(&data->pages, pagecount)) {
		data->header = hdr;
		atomic_inc(&hdr->refcnt);
	} else {
		if (data != prealloc)
			kfree(data);
		data = NULL;
	}
out:
	return data;
}
