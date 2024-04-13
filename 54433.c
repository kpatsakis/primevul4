void LYShowBadHTML(const char *message)
{
    if (dump_output_immediately && dump_to_stderr)
	fprintf(stderr, "%s", message);

    switch ((enumBadHtml) cfg_bad_html) {
    case BAD_HTML_IGNORE:
	break;
    case BAD_HTML_TRACE:
    case BAD_HTML_MESSAGE:
    case BAD_HTML_WARN:
	CTRACE((tfp, "%s", message));
	break;
    }

    switch ((enumBadHtml) cfg_bad_html) {
    case BAD_HTML_IGNORE:
    case BAD_HTML_TRACE:
    case BAD_HTML_WARN:
	break;
    case BAD_HTML_MESSAGE:
	LYstore_message(message);
	break;
    }
}
