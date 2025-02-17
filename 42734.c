static void setup_cifs_sb(struct smb_vol *pvolume_info,
			  struct cifs_sb_info *cifs_sb)
{
	if (pvolume_info->rsize > CIFSMaxBufSize) {
		cERROR(1, "rsize %d too large, using MaxBufSize",
			pvolume_info->rsize);
		cifs_sb->rsize = CIFSMaxBufSize;
	} else if ((pvolume_info->rsize) &&
			(pvolume_info->rsize <= CIFSMaxBufSize))
		cifs_sb->rsize = pvolume_info->rsize;
	else /* default */
		cifs_sb->rsize = CIFSMaxBufSize;

	if (pvolume_info->wsize > PAGEVEC_SIZE * PAGE_CACHE_SIZE) {
		cERROR(1, "wsize %d too large, using 4096 instead",
			  pvolume_info->wsize);
		cifs_sb->wsize = 4096;
	} else if (pvolume_info->wsize)
		cifs_sb->wsize = pvolume_info->wsize;
	else
		cifs_sb->wsize = min_t(const int,
					PAGEVEC_SIZE * PAGE_CACHE_SIZE,
					127*1024);
		/* old default of CIFSMaxBufSize was too small now
		   that SMB Write2 can send multiple pages in kvec.
		   RFC1001 does not describe what happens when frame
		   bigger than 128K is sent so use that as max in
		   conjunction with 52K kvec constraint on arch with 4K
		   page size  */

	if (cifs_sb->rsize < 2048) {
		cifs_sb->rsize = 2048;
		/* Windows ME may prefer this */
		cFYI(1, "readsize set to minimum: 2048");
	}
	/* calculate prepath */
	cifs_sb->prepath = pvolume_info->prepath;
	if (cifs_sb->prepath) {
		cifs_sb->prepathlen = strlen(cifs_sb->prepath);
		/* we can not convert the / to \ in the path
		separators in the prefixpath yet because we do not
		know (until reset_cifs_unix_caps is called later)
		whether POSIX PATH CAP is available. We normalize
		the / to \ after reset_cifs_unix_caps is called */
		pvolume_info->prepath = NULL;
	} else
		cifs_sb->prepathlen = 0;
	cifs_sb->mnt_uid = pvolume_info->linux_uid;
	cifs_sb->mnt_gid = pvolume_info->linux_gid;
	cifs_sb->mnt_file_mode = pvolume_info->file_mode;
	cifs_sb->mnt_dir_mode = pvolume_info->dir_mode;
	cFYI(1, "file mode: 0x%x  dir mode: 0x%x",
		cifs_sb->mnt_file_mode, cifs_sb->mnt_dir_mode);

	if (pvolume_info->noperm)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_NO_PERM;
	if (pvolume_info->setuids)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_SET_UID;
	if (pvolume_info->server_ino)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_SERVER_INUM;
	if (pvolume_info->remap)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_MAP_SPECIAL_CHR;
	if (pvolume_info->no_xattr)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_NO_XATTR;
	if (pvolume_info->sfu_emul)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_UNX_EMUL;
	if (pvolume_info->nobrl)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_NO_BRL;
	if (pvolume_info->nostrictsync)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_NOSSYNC;
	if (pvolume_info->mand_lock)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_NOPOSIXBRL;
	if (pvolume_info->cifs_acl)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_CIFS_ACL;
	if (pvolume_info->override_uid)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_OVERR_UID;
	if (pvolume_info->override_gid)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_OVERR_GID;
	if (pvolume_info->dynperm)
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_DYNPERM;
	if (pvolume_info->direct_io) {
		cFYI(1, "mounting share using direct i/o");
		cifs_sb->mnt_cifs_flags |= CIFS_MOUNT_DIRECT_IO;
	}

	if ((pvolume_info->cifs_acl) && (pvolume_info->dynperm))
		cERROR(1, "mount option dynperm ignored if cifsacl "
			   "mount option supported");
}
