xml_parse_file_ext2(struct xar *xar, const char *name)
{
	const char *flag = NULL;

	if (strcmp(name, "SecureDeletion") == 0) {
		xar->xmlsts = FILE_EXT2_SecureDeletion;
		flag = "securedeletion";
	}
	else if (strcmp(name, "Undelete") == 0) {
		xar->xmlsts = FILE_EXT2_Undelete;
		flag = "nouunlink";
	}
	else if (strcmp(name, "Compress") == 0) {
		xar->xmlsts = FILE_EXT2_Compress;
		flag = "compress";
	}
	else if (strcmp(name, "Synchronous") == 0) {
		xar->xmlsts = FILE_EXT2_Synchronous;
		flag = "sync";
	}
	else if (strcmp(name, "Immutable") == 0) {
		xar->xmlsts = FILE_EXT2_Immutable;
		flag = "simmutable";
	}
	else if (strcmp(name, "AppendOnly") == 0) {
		xar->xmlsts = FILE_EXT2_AppendOnly;
		flag = "sappend";
	}
	else if (strcmp(name, "NoDump") == 0) {
		xar->xmlsts = FILE_EXT2_NoDump;
		flag = "nodump";
	}
	else if (strcmp(name, "NoAtime") == 0) {
		xar->xmlsts = FILE_EXT2_NoAtime;
		flag = "noatime";
	}
	else if (strcmp(name, "CompDirty") == 0) {
		xar->xmlsts = FILE_EXT2_CompDirty;
		flag = "compdirty";
	}
	else if (strcmp(name, "CompBlock") == 0) {
		xar->xmlsts = FILE_EXT2_CompBlock;
		flag = "comprblk";
	}
	else if (strcmp(name, "NoCompBlock") == 0) {
		xar->xmlsts = FILE_EXT2_NoCompBlock;
		flag = "nocomprblk";
	}
	else if (strcmp(name, "CompError") == 0) {
		xar->xmlsts = FILE_EXT2_CompError;
		flag = "comperr";
	}
	else if (strcmp(name, "BTree") == 0) {
		xar->xmlsts = FILE_EXT2_BTree;
		flag = "btree";
	}
	else if (strcmp(name, "HashIndexed") == 0) {
		xar->xmlsts = FILE_EXT2_HashIndexed;
		flag = "hashidx";
	}
	else if (strcmp(name, "iMagic") == 0) {
		xar->xmlsts = FILE_EXT2_iMagic;
		flag = "imagic";
	}
	else if (strcmp(name, "Journaled") == 0) {
		xar->xmlsts = FILE_EXT2_Journaled;
		flag = "journal";
	}
	else if (strcmp(name, "NoTail") == 0) {
		xar->xmlsts = FILE_EXT2_NoTail;
		flag = "notail";
	}
	else if (strcmp(name, "DirSync") == 0) {
		xar->xmlsts = FILE_EXT2_DirSync;
		flag = "dirsync";
	}
	else if (strcmp(name, "TopDir") == 0) {
		xar->xmlsts = FILE_EXT2_TopDir;
		flag = "topdir";
	}
	else if (strcmp(name, "Reserved") == 0) {
		xar->xmlsts = FILE_EXT2_Reserved;
		flag = "reserved";
	}

	if (flag == NULL)
		return (0);
	if (archive_strlen(&(xar->file->fflags_text)) > 0)
		archive_strappend_char(&(xar->file->fflags_text), ',');
	archive_strcat(&(xar->file->fflags_text), flag);
	return (1);
}
