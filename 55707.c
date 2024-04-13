xml_end(void *userData, const char *name)
{
	struct archive_read *a;
	struct xar *xar;

	a = (struct archive_read *)userData;
	xar = (struct xar *)(a->format->data);

#if DEBUG
	fprintf(stderr, "xml_end:[%s]\n", name);
#endif
	switch (xar->xmlsts) {
	case INIT:
		break;
	case XAR:
		if (strcmp(name, "xar") == 0)
			xar->xmlsts = INIT;
		break;
	case TOC:
		if (strcmp(name, "toc") == 0)
			xar->xmlsts = XAR;
		break;
	case TOC_CREATION_TIME:
		if (strcmp(name, "creation-time") == 0)
			xar->xmlsts = TOC;
		break;
	case TOC_CHECKSUM:
		if (strcmp(name, "checksum") == 0)
			xar->xmlsts = TOC;
		break;
	case TOC_CHECKSUM_OFFSET:
		if (strcmp(name, "offset") == 0)
			xar->xmlsts = TOC_CHECKSUM;
		break;
	case TOC_CHECKSUM_SIZE:
		if (strcmp(name, "size") == 0)
			xar->xmlsts = TOC_CHECKSUM;
		break;
	case TOC_FILE:
		if (strcmp(name, "file") == 0) {
			if (xar->file->parent != NULL &&
			    ((xar->file->mode & AE_IFMT) == AE_IFDIR))
				xar->file->parent->subdirs++;
			xar->file = xar->file->parent;
			if (xar->file == NULL)
				xar->xmlsts = TOC;
		}
		break;
	case FILE_DATA:
		if (strcmp(name, "data") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_DATA_LENGTH:
		if (strcmp(name, "length") == 0)
			xar->xmlsts = FILE_DATA;
		break;
	case FILE_DATA_OFFSET:
		if (strcmp(name, "offset") == 0)
			xar->xmlsts = FILE_DATA;
		break;
	case FILE_DATA_SIZE:
		if (strcmp(name, "size") == 0)
			xar->xmlsts = FILE_DATA;
		break;
	case FILE_DATA_ENCODING:
		if (strcmp(name, "encoding") == 0)
			xar->xmlsts = FILE_DATA;
		break;
	case FILE_DATA_A_CHECKSUM:
		if (strcmp(name, "archived-checksum") == 0)
			xar->xmlsts = FILE_DATA;
		break;
	case FILE_DATA_E_CHECKSUM:
		if (strcmp(name, "extracted-checksum") == 0)
			xar->xmlsts = FILE_DATA;
		break;
	case FILE_DATA_CONTENT:
		if (strcmp(name, "content") == 0)
			xar->xmlsts = FILE_DATA;
		break;
	case FILE_EA:
		if (strcmp(name, "ea") == 0) {
			xar->xmlsts = TOC_FILE;
			xar->xattr = NULL;
		}
		break;
	case FILE_EA_LENGTH:
		if (strcmp(name, "length") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_EA_OFFSET:
		if (strcmp(name, "offset") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_EA_SIZE:
		if (strcmp(name, "size") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_EA_ENCODING:
		if (strcmp(name, "encoding") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_EA_A_CHECKSUM:
		if (strcmp(name, "archived-checksum") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_EA_E_CHECKSUM:
		if (strcmp(name, "extracted-checksum") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_EA_NAME:
		if (strcmp(name, "name") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_EA_FSTYPE:
		if (strcmp(name, "fstype") == 0)
			xar->xmlsts = FILE_EA;
		break;
	case FILE_CTIME:
		if (strcmp(name, "ctime") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_MTIME:
		if (strcmp(name, "mtime") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_ATIME:
		if (strcmp(name, "atime") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_GROUP:
		if (strcmp(name, "group") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_GID:
		if (strcmp(name, "gid") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_USER:
		if (strcmp(name, "user") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_UID:
		if (strcmp(name, "uid") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_MODE:
		if (strcmp(name, "mode") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_DEVICE:
		if (strcmp(name, "device") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_DEVICE_MAJOR:
		if (strcmp(name, "major") == 0)
			xar->xmlsts = FILE_DEVICE;
		break;
	case FILE_DEVICE_MINOR:
		if (strcmp(name, "minor") == 0)
			xar->xmlsts = FILE_DEVICE;
		break;
	case FILE_DEVICENO:
		if (strcmp(name, "deviceno") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_INODE:
		if (strcmp(name, "inode") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_LINK:
		if (strcmp(name, "link") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_TYPE:
		if (strcmp(name, "type") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_NAME:
		if (strcmp(name, "name") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_ACL:
		if (strcmp(name, "acl") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_ACL_DEFAULT:
		if (strcmp(name, "default") == 0)
			xar->xmlsts = FILE_ACL;
		break;
	case FILE_ACL_ACCESS:
		if (strcmp(name, "access") == 0)
			xar->xmlsts = FILE_ACL;
		break;
	case FILE_ACL_APPLEEXTENDED:
		if (strcmp(name, "appleextended") == 0)
			xar->xmlsts = FILE_ACL;
		break;
	case FILE_FLAGS:
		if (strcmp(name, "flags") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_FLAGS_USER_NODUMP:
		if (strcmp(name, "UserNoDump") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_USER_IMMUTABLE:
		if (strcmp(name, "UserImmutable") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_USER_APPEND:
		if (strcmp(name, "UserAppend") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_USER_OPAQUE:
		if (strcmp(name, "UserOpaque") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_USER_NOUNLINK:
		if (strcmp(name, "UserNoUnlink") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_SYS_ARCHIVED:
		if (strcmp(name, "SystemArchived") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_SYS_IMMUTABLE:
		if (strcmp(name, "SystemImmutable") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_SYS_APPEND:
		if (strcmp(name, "SystemAppend") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_SYS_NOUNLINK:
		if (strcmp(name, "SystemNoUnlink") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_FLAGS_SYS_SNAPSHOT:
		if (strcmp(name, "SystemSnapshot") == 0)
			xar->xmlsts = FILE_FLAGS;
		break;
	case FILE_EXT2:
		if (strcmp(name, "ext2") == 0)
			xar->xmlsts = TOC_FILE;
		break;
	case FILE_EXT2_SecureDeletion:
		if (strcmp(name, "SecureDeletion") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_Undelete:
		if (strcmp(name, "Undelete") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_Compress:
		if (strcmp(name, "Compress") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_Synchronous:
		if (strcmp(name, "Synchronous") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_Immutable:
		if (strcmp(name, "Immutable") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_AppendOnly:
		if (strcmp(name, "AppendOnly") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_NoDump:
		if (strcmp(name, "NoDump") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_NoAtime:
		if (strcmp(name, "NoAtime") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_CompDirty:
		if (strcmp(name, "CompDirty") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_CompBlock:
		if (strcmp(name, "CompBlock") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_NoCompBlock:
		if (strcmp(name, "NoCompBlock") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_CompError:
		if (strcmp(name, "CompError") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_BTree:
		if (strcmp(name, "BTree") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_HashIndexed:
		if (strcmp(name, "HashIndexed") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_iMagic:
		if (strcmp(name, "iMagic") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_Journaled:
		if (strcmp(name, "Journaled") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_NoTail:
		if (strcmp(name, "NoTail") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_DirSync:
		if (strcmp(name, "DirSync") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_TopDir:
		if (strcmp(name, "TopDir") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case FILE_EXT2_Reserved:
		if (strcmp(name, "Reserved") == 0)
			xar->xmlsts = FILE_EXT2;
		break;
	case UNKNOWN:
		unknowntag_end(xar, name);
		break;
	}
}
