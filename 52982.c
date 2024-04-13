static void read_revisions_from_stdin(struct rev_info *revs,
				      struct cmdline_pathspec *prune)
{
	struct strbuf sb;
	int seen_dashdash = 0;
	int save_warning;

	save_warning = warn_on_object_refname_ambiguity;
	warn_on_object_refname_ambiguity = 0;

	strbuf_init(&sb, 1000);
	while (strbuf_getwholeline(&sb, stdin, '\n') != EOF) {
		int len = sb.len;
		if (len && sb.buf[len - 1] == '\n')
			sb.buf[--len] = '\0';
		if (!len)
			break;
		if (sb.buf[0] == '-') {
			if (len == 2 && sb.buf[1] == '-') {
				seen_dashdash = 1;
				break;
			}
			die("options not supported in --stdin mode");
		}
		if (handle_revision_arg(sb.buf, revs, 0,
					REVARG_CANNOT_BE_FILENAME))
			die("bad revision '%s'", sb.buf);
	}
	if (seen_dashdash)
		read_pathspec_from_stdin(revs, &sb, prune);

	strbuf_release(&sb);
	warn_on_object_refname_ambiguity = save_warning;
}
