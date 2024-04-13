xml_data(void *userData, const char *s, int len)
{
	struct archive_read *a;
	struct xar *xar;

	a = (struct archive_read *)userData;
	xar = (struct xar *)(a->format->data);

#if DEBUG
	{
		char buff[1024];
		if (len > (int)(sizeof(buff)-1))
			len = (int)(sizeof(buff)-1);
		strncpy(buff, s, len);
		buff[len] = 0;
		fprintf(stderr, "\tlen=%d:\"%s\"\n", len, buff);
	}
#endif
	switch (xar->xmlsts) {
	case TOC_CHECKSUM_OFFSET:
		xar->toc_chksum_offset = atol10(s, len);
		break;
	case TOC_CHECKSUM_SIZE:
		xar->toc_chksum_size = atol10(s, len);
		break;
	default:
		break;
	}
	if (xar->file == NULL)
		return;

	switch (xar->xmlsts) {
	case FILE_NAME:
		if (xar->file->parent != NULL) {
			archive_string_concat(&(xar->file->pathname),
			    &(xar->file->parent->pathname));
			archive_strappend_char(&(xar->file->pathname), '/');
		}
		xar->file->has |= HAS_PATHNAME;
		if (xar->base64text) {
			strappend_base64(xar,
			    &(xar->file->pathname), s, len);
		} else
			archive_strncat(&(xar->file->pathname), s, len);
		break;
	case FILE_LINK:
		xar->file->has |= HAS_SYMLINK;
		archive_strncpy(&(xar->file->symlink), s, len);
		break;
	case FILE_TYPE:
		if (strncmp("file", s, len) == 0 ||
		    strncmp("hardlink", s, len) == 0)
			xar->file->mode =
			    (xar->file->mode & ~AE_IFMT) | AE_IFREG;
		if (strncmp("directory", s, len) == 0)
			xar->file->mode =
			    (xar->file->mode & ~AE_IFMT) | AE_IFDIR;
		if (strncmp("symlink", s, len) == 0)
			xar->file->mode =
			    (xar->file->mode & ~AE_IFMT) | AE_IFLNK;
		if (strncmp("character special", s, len) == 0)
			xar->file->mode =
			    (xar->file->mode & ~AE_IFMT) | AE_IFCHR;
		if (strncmp("block special", s, len) == 0)
			xar->file->mode =
			    (xar->file->mode & ~AE_IFMT) | AE_IFBLK;
		if (strncmp("socket", s, len) == 0)
			xar->file->mode =
			    (xar->file->mode & ~AE_IFMT) | AE_IFSOCK;
		if (strncmp("fifo", s, len) == 0)
			xar->file->mode =
			    (xar->file->mode & ~AE_IFMT) | AE_IFIFO;
		xar->file->has |= HAS_TYPE;
		break;
	case FILE_INODE:
		xar->file->has |= HAS_INO;
		xar->file->ino64 = atol10(s, len);
		break;
	case FILE_DEVICE_MAJOR:
		xar->file->has |= HAS_DEVMAJOR;
		xar->file->devmajor = (dev_t)atol10(s, len);
		break;
	case FILE_DEVICE_MINOR:
		xar->file->has |= HAS_DEVMINOR;
		xar->file->devminor = (dev_t)atol10(s, len);
		break;
	case FILE_DEVICENO:
		xar->file->has |= HAS_DEV;
		xar->file->dev = (dev_t)atol10(s, len);
		break;
	case FILE_MODE:
		xar->file->has |= HAS_MODE;
		xar->file->mode =
		    (xar->file->mode & AE_IFMT) |
		    ((mode_t)(atol8(s, len)) & ~AE_IFMT);
		break;
	case FILE_GROUP:
		xar->file->has |= HAS_GID;
		archive_strncpy(&(xar->file->gname), s, len);
		break;
	case FILE_GID:
		xar->file->has |= HAS_GID;
		xar->file->gid = atol10(s, len);
		break;
	case FILE_USER:
		xar->file->has |= HAS_UID;
		archive_strncpy(&(xar->file->uname), s, len);
		break;
	case FILE_UID:
		xar->file->has |= HAS_UID;
		xar->file->uid = atol10(s, len);
		break;
	case FILE_CTIME:
		xar->file->has |= HAS_TIME;
		xar->file->ctime = parse_time(s, len);
		break;
	case FILE_MTIME:
		xar->file->has |= HAS_TIME;
		xar->file->mtime = parse_time(s, len);
		break;
	case FILE_ATIME:
		xar->file->has |= HAS_TIME;
		xar->file->atime = parse_time(s, len);
		break;
	case FILE_DATA_LENGTH:
		xar->file->has |= HAS_DATA;
		xar->file->length = atol10(s, len);
		break;
	case FILE_DATA_OFFSET:
		xar->file->has |= HAS_DATA;
		xar->file->offset = atol10(s, len);
		break;
	case FILE_DATA_SIZE:
		xar->file->has |= HAS_DATA;
		xar->file->size = atol10(s, len);
		break;
	case FILE_DATA_A_CHECKSUM:
		xar->file->a_sum.len = atohex(xar->file->a_sum.val,
		    sizeof(xar->file->a_sum.val), s, len);
		break;
	case FILE_DATA_E_CHECKSUM:
		xar->file->e_sum.len = atohex(xar->file->e_sum.val,
		    sizeof(xar->file->e_sum.val), s, len);
		break;
	case FILE_EA_LENGTH:
		xar->file->has |= HAS_XATTR;
		xar->xattr->length = atol10(s, len);
		break;
	case FILE_EA_OFFSET:
		xar->file->has |= HAS_XATTR;
		xar->xattr->offset = atol10(s, len);
		break;
	case FILE_EA_SIZE:
		xar->file->has |= HAS_XATTR;
		xar->xattr->size = atol10(s, len);
		break;
	case FILE_EA_A_CHECKSUM:
		xar->file->has |= HAS_XATTR;
		xar->xattr->a_sum.len = atohex(xar->xattr->a_sum.val,
		    sizeof(xar->xattr->a_sum.val), s, len);
		break;
	case FILE_EA_E_CHECKSUM:
		xar->file->has |= HAS_XATTR;
		xar->xattr->e_sum.len = atohex(xar->xattr->e_sum.val,
		    sizeof(xar->xattr->e_sum.val), s, len);
		break;
	case FILE_EA_NAME:
		xar->file->has |= HAS_XATTR;
		archive_strncpy(&(xar->xattr->name), s, len);
		break;
	case FILE_EA_FSTYPE:
		xar->file->has |= HAS_XATTR;
		archive_strncpy(&(xar->xattr->fstype), s, len);
		break;
		break;
	case FILE_ACL_DEFAULT:
	case FILE_ACL_ACCESS:
	case FILE_ACL_APPLEEXTENDED:
		xar->file->has |= HAS_ACL;
		/* TODO */
		break;
	case INIT:
	case XAR:
	case TOC:
	case TOC_CREATION_TIME:
	case TOC_CHECKSUM:
	case TOC_CHECKSUM_OFFSET:
	case TOC_CHECKSUM_SIZE:
	case TOC_FILE:
	case FILE_DATA:
	case FILE_DATA_ENCODING:
	case FILE_DATA_CONTENT:
	case FILE_DEVICE:
	case FILE_EA:
	case FILE_EA_ENCODING:
	case FILE_ACL:
	case FILE_FLAGS:
	case FILE_FLAGS_USER_NODUMP:
	case FILE_FLAGS_USER_IMMUTABLE:
	case FILE_FLAGS_USER_APPEND:
	case FILE_FLAGS_USER_OPAQUE:
	case FILE_FLAGS_USER_NOUNLINK:
	case FILE_FLAGS_SYS_ARCHIVED:
	case FILE_FLAGS_SYS_IMMUTABLE:
	case FILE_FLAGS_SYS_APPEND:
	case FILE_FLAGS_SYS_NOUNLINK:
	case FILE_FLAGS_SYS_SNAPSHOT:
	case FILE_EXT2:
	case FILE_EXT2_SecureDeletion:
	case FILE_EXT2_Undelete:
	case FILE_EXT2_Compress:
	case FILE_EXT2_Synchronous:
	case FILE_EXT2_Immutable:
	case FILE_EXT2_AppendOnly:
	case FILE_EXT2_NoDump:
	case FILE_EXT2_NoAtime:
	case FILE_EXT2_CompDirty:
	case FILE_EXT2_CompBlock:
	case FILE_EXT2_NoCompBlock:
	case FILE_EXT2_CompError:
	case FILE_EXT2_BTree:
	case FILE_EXT2_HashIndexed:
	case FILE_EXT2_iMagic:
	case FILE_EXT2_Journaled:
	case FILE_EXT2_NoTail:
	case FILE_EXT2_DirSync:
	case FILE_EXT2_TopDir:
	case FILE_EXT2_Reserved:
	case UNKNOWN:
		break;
	}
}
