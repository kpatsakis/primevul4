static int __ffs_data_got_descs(struct ffs_data *ffs,
				char *const _data, size_t len)
{
	char *data = _data, *raw_descs;
	unsigned os_descs_count = 0, counts[3], flags;
	int ret = -EINVAL, i;
	struct ffs_desc_helper helper;

	ENTER();

	if (get_unaligned_le32(data + 4) != len)
		goto error;

	switch (get_unaligned_le32(data)) {
	case FUNCTIONFS_DESCRIPTORS_MAGIC:
		flags = FUNCTIONFS_HAS_FS_DESC | FUNCTIONFS_HAS_HS_DESC;
		data += 8;
		len  -= 8;
		break;
	case FUNCTIONFS_DESCRIPTORS_MAGIC_V2:
		flags = get_unaligned_le32(data + 8);
		ffs->user_flags = flags;
		if (flags & ~(FUNCTIONFS_HAS_FS_DESC |
			      FUNCTIONFS_HAS_HS_DESC |
			      FUNCTIONFS_HAS_SS_DESC |
			      FUNCTIONFS_HAS_MS_OS_DESC |
			      FUNCTIONFS_VIRTUAL_ADDR |
			      FUNCTIONFS_EVENTFD)) {
			ret = -ENOSYS;
			goto error;
		}
		data += 12;
		len  -= 12;
		break;
	default:
		goto error;
	}

	if (flags & FUNCTIONFS_EVENTFD) {
		if (len < 4)
			goto error;
		ffs->ffs_eventfd =
			eventfd_ctx_fdget((int)get_unaligned_le32(data));
		if (IS_ERR(ffs->ffs_eventfd)) {
			ret = PTR_ERR(ffs->ffs_eventfd);
			ffs->ffs_eventfd = NULL;
			goto error;
		}
		data += 4;
		len  -= 4;
	}

	/* Read fs_count, hs_count and ss_count (if present) */
	for (i = 0; i < 3; ++i) {
		if (!(flags & (1 << i))) {
			counts[i] = 0;
		} else if (len < 4) {
			goto error;
		} else {
			counts[i] = get_unaligned_le32(data);
			data += 4;
			len  -= 4;
		}
	}
	if (flags & (1 << i)) {
		os_descs_count = get_unaligned_le32(data);
		data += 4;
		len -= 4;
	};

	/* Read descriptors */
	raw_descs = data;
	helper.ffs = ffs;
	for (i = 0; i < 3; ++i) {
		if (!counts[i])
			continue;
		helper.interfaces_count = 0;
		helper.eps_count = 0;
		ret = ffs_do_descs(counts[i], data, len,
				   __ffs_data_do_entity, &helper);
		if (ret < 0)
			goto error;
		if (!ffs->eps_count && !ffs->interfaces_count) {
			ffs->eps_count = helper.eps_count;
			ffs->interfaces_count = helper.interfaces_count;
		} else {
			if (ffs->eps_count != helper.eps_count) {
				ret = -EINVAL;
				goto error;
			}
			if (ffs->interfaces_count != helper.interfaces_count) {
				ret = -EINVAL;
				goto error;
			}
		}
		data += ret;
		len  -= ret;
	}
	if (os_descs_count) {
		ret = ffs_do_os_descs(os_descs_count, data, len,
				      __ffs_data_do_os_desc, ffs);
		if (ret < 0)
			goto error;
		data += ret;
		len -= ret;
	}

	if (raw_descs == data || len) {
		ret = -EINVAL;
		goto error;
	}

	ffs->raw_descs_data	= _data;
	ffs->raw_descs		= raw_descs;
	ffs->raw_descs_length	= data - raw_descs;
	ffs->fs_descs_count	= counts[0];
	ffs->hs_descs_count	= counts[1];
	ffs->ss_descs_count	= counts[2];
	ffs->ms_os_descs_count	= os_descs_count;

	return 0;

error:
	kfree(_data);
	return ret;
}
