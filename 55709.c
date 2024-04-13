xml_parse_file_flags(struct xar *xar, const char *name)
{
	const char *flag = NULL;

	if (strcmp(name, "UserNoDump") == 0) {
		xar->xmlsts = FILE_FLAGS_USER_NODUMP;
		flag = "nodump";
	}
	else if (strcmp(name, "UserImmutable") == 0) {
		xar->xmlsts = FILE_FLAGS_USER_IMMUTABLE;
		flag = "uimmutable";
	}
	else if (strcmp(name, "UserAppend") == 0) {
		xar->xmlsts = FILE_FLAGS_USER_APPEND;
		flag = "uappend";
	}
	else if (strcmp(name, "UserOpaque") == 0) {
		xar->xmlsts = FILE_FLAGS_USER_OPAQUE;
		flag = "opaque";
	}
	else if (strcmp(name, "UserNoUnlink") == 0) {
		xar->xmlsts = FILE_FLAGS_USER_NOUNLINK;
		flag = "nouunlink";
	}
	else if (strcmp(name, "SystemArchived") == 0) {
		xar->xmlsts = FILE_FLAGS_SYS_ARCHIVED;
		flag = "archived";
	}
	else if (strcmp(name, "SystemImmutable") == 0) {
		xar->xmlsts = FILE_FLAGS_SYS_IMMUTABLE;
		flag = "simmutable";
	}
	else if (strcmp(name, "SystemAppend") == 0) {
		xar->xmlsts = FILE_FLAGS_SYS_APPEND;
		flag = "sappend";
	}
	else if (strcmp(name, "SystemNoUnlink") == 0) {
		xar->xmlsts = FILE_FLAGS_SYS_NOUNLINK;
		flag = "nosunlink";
	}
	else if (strcmp(name, "SystemSnapshot") == 0) {
		xar->xmlsts = FILE_FLAGS_SYS_SNAPSHOT;
		flag = "snapshot";
	}

	if (flag == NULL)
		return (0);
	xar->file->has |= HAS_FFLAGS;
	if (archive_strlen(&(xar->file->fflags_text)) > 0)
		archive_strappend_char(&(xar->file->fflags_text), ',');
	archive_strcat(&(xar->file->fflags_text), flag);
	return (1);
}
