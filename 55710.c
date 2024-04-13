xml_start(struct archive_read *a, const char *name, struct xmlattr_list *list)
{
	struct xar *xar;
	struct xmlattr *attr;

	xar = (struct xar *)(a->format->data);

#if DEBUG
	fprintf(stderr, "xml_sta:[%s]\n", name);
	for (attr = list->first; attr != NULL; attr = attr->next)
		fprintf(stderr, "    attr:\"%s\"=\"%s\"\n",
		    attr->name, attr->value);
#endif
	xar->base64text = 0;
	switch (xar->xmlsts) {
	case INIT:
		if (strcmp(name, "xar") == 0)
			xar->xmlsts = XAR;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case XAR:
		if (strcmp(name, "toc") == 0)
			xar->xmlsts = TOC;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case TOC:
		if (strcmp(name, "creation-time") == 0)
			xar->xmlsts = TOC_CREATION_TIME;
		else if (strcmp(name, "checksum") == 0)
			xar->xmlsts = TOC_CHECKSUM;
		else if (strcmp(name, "file") == 0) {
			if (file_new(a, xar, list) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
			xar->xmlsts = TOC_FILE;
		}
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case TOC_CHECKSUM:
		if (strcmp(name, "offset") == 0)
			xar->xmlsts = TOC_CHECKSUM_OFFSET;
		else if (strcmp(name, "size") == 0)
			xar->xmlsts = TOC_CHECKSUM_SIZE;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case TOC_FILE:
		if (strcmp(name, "file") == 0) {
			if (file_new(a, xar, list) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		}
		else if (strcmp(name, "data") == 0)
			xar->xmlsts = FILE_DATA;
		else if (strcmp(name, "ea") == 0) {
			if (xattr_new(a, xar, list) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
			xar->xmlsts = FILE_EA;
		}
		else if (strcmp(name, "ctime") == 0)
			xar->xmlsts = FILE_CTIME;
		else if (strcmp(name, "mtime") == 0)
			xar->xmlsts = FILE_MTIME;
		else if (strcmp(name, "atime") == 0)
			xar->xmlsts = FILE_ATIME;
		else if (strcmp(name, "group") == 0)
			xar->xmlsts = FILE_GROUP;
		else if (strcmp(name, "gid") == 0)
			xar->xmlsts = FILE_GID;
		else if (strcmp(name, "user") == 0)
			xar->xmlsts = FILE_USER;
		else if (strcmp(name, "uid") == 0)
			xar->xmlsts = FILE_UID;
		else if (strcmp(name, "mode") == 0)
			xar->xmlsts = FILE_MODE;
		else if (strcmp(name, "device") == 0)
			xar->xmlsts = FILE_DEVICE;
		else if (strcmp(name, "deviceno") == 0)
			xar->xmlsts = FILE_DEVICENO;
		else if (strcmp(name, "inode") == 0)
			xar->xmlsts = FILE_INODE;
		else if (strcmp(name, "link") == 0)
			xar->xmlsts = FILE_LINK;
		else if (strcmp(name, "type") == 0) {
			xar->xmlsts = FILE_TYPE;
			for (attr = list->first; attr != NULL;
			    attr = attr->next) {
				if (strcmp(attr->name, "link") != 0)
					continue;
				if (strcmp(attr->value, "original") == 0) {
					xar->file->hdnext = xar->hdlink_orgs;
					xar->hdlink_orgs = xar->file;
				} else {
					xar->file->link = (unsigned)atol10(attr->value,
					    strlen(attr->value));
					if (xar->file->link > 0)
						if (add_link(a, xar, xar->file) != ARCHIVE_OK) {
							return (ARCHIVE_FATAL);
						};
				}
			}
		}
		else if (strcmp(name, "name") == 0) {
			xar->xmlsts = FILE_NAME;
			for (attr = list->first; attr != NULL;
			    attr = attr->next) {
				if (strcmp(attr->name, "enctype") == 0 &&
				    strcmp(attr->value, "base64") == 0)
					xar->base64text = 1;
			}
		}
		else if (strcmp(name, "acl") == 0)
			xar->xmlsts = FILE_ACL;
		else if (strcmp(name, "flags") == 0)
			xar->xmlsts = FILE_FLAGS;
		else if (strcmp(name, "ext2") == 0)
			xar->xmlsts = FILE_EXT2;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case FILE_DATA:
		if (strcmp(name, "length") == 0)
			xar->xmlsts = FILE_DATA_LENGTH;
		else if (strcmp(name, "offset") == 0)
			xar->xmlsts = FILE_DATA_OFFSET;
		else if (strcmp(name, "size") == 0)
			xar->xmlsts = FILE_DATA_SIZE;
		else if (strcmp(name, "encoding") == 0) {
			xar->xmlsts = FILE_DATA_ENCODING;
			xar->file->encoding = getencoding(list);
		}
		else if (strcmp(name, "archived-checksum") == 0) {
			xar->xmlsts = FILE_DATA_A_CHECKSUM;
			xar->file->a_sum.alg = getsumalgorithm(list);
		}
		else if (strcmp(name, "extracted-checksum") == 0) {
			xar->xmlsts = FILE_DATA_E_CHECKSUM;
			xar->file->e_sum.alg = getsumalgorithm(list);
		}
		else if (strcmp(name, "content") == 0)
			xar->xmlsts = FILE_DATA_CONTENT;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case FILE_DEVICE:
		if (strcmp(name, "major") == 0)
			xar->xmlsts = FILE_DEVICE_MAJOR;
		else if (strcmp(name, "minor") == 0)
			xar->xmlsts = FILE_DEVICE_MINOR;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case FILE_DATA_CONTENT:
		if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
		break;
	case FILE_EA:
		if (strcmp(name, "length") == 0)
			xar->xmlsts = FILE_EA_LENGTH;
		else if (strcmp(name, "offset") == 0)
			xar->xmlsts = FILE_EA_OFFSET;
		else if (strcmp(name, "size") == 0)
			xar->xmlsts = FILE_EA_SIZE;
		else if (strcmp(name, "encoding") == 0) {
			xar->xmlsts = FILE_EA_ENCODING;
			xar->xattr->encoding = getencoding(list);
		} else if (strcmp(name, "archived-checksum") == 0)
			xar->xmlsts = FILE_EA_A_CHECKSUM;
		else if (strcmp(name, "extracted-checksum") == 0)
			xar->xmlsts = FILE_EA_E_CHECKSUM;
		else if (strcmp(name, "name") == 0)
			xar->xmlsts = FILE_EA_NAME;
		else if (strcmp(name, "fstype") == 0)
			xar->xmlsts = FILE_EA_FSTYPE;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case FILE_ACL:
		if (strcmp(name, "appleextended") == 0)
			xar->xmlsts = FILE_ACL_APPLEEXTENDED;
		else if (strcmp(name, "default") == 0)
			xar->xmlsts = FILE_ACL_DEFAULT;
		else if (strcmp(name, "access") == 0)
			xar->xmlsts = FILE_ACL_ACCESS;
		else
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case FILE_FLAGS:
		if (!xml_parse_file_flags(xar, name))
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case FILE_EXT2:
		if (!xml_parse_file_ext2(xar, name))
			if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		break;
	case TOC_CREATION_TIME:
	case TOC_CHECKSUM_OFFSET:
	case TOC_CHECKSUM_SIZE:
	case FILE_DATA_LENGTH:
	case FILE_DATA_OFFSET:
	case FILE_DATA_SIZE:
	case FILE_DATA_ENCODING:
	case FILE_DATA_A_CHECKSUM:
	case FILE_DATA_E_CHECKSUM:
	case FILE_EA_LENGTH:
	case FILE_EA_OFFSET:
	case FILE_EA_SIZE:
	case FILE_EA_ENCODING:
	case FILE_EA_A_CHECKSUM:
	case FILE_EA_E_CHECKSUM:
	case FILE_EA_NAME:
	case FILE_EA_FSTYPE:
	case FILE_CTIME:
	case FILE_MTIME:
	case FILE_ATIME:
	case FILE_GROUP:
	case FILE_GID:
	case FILE_USER:
	case FILE_UID:
	case FILE_INODE:
	case FILE_DEVICE_MAJOR:
	case FILE_DEVICE_MINOR:
	case FILE_DEVICENO:
	case FILE_MODE:
	case FILE_TYPE:
	case FILE_LINK:
	case FILE_NAME:
	case FILE_ACL_DEFAULT:
	case FILE_ACL_ACCESS:
	case FILE_ACL_APPLEEXTENDED:
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
		if (unknowntag_start(a, xar, name) != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
		break;
	}
	return (ARCHIVE_OK);
}
