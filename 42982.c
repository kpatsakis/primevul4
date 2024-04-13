file_error_core(struct magic_set *ms, int error, const char *f, va_list va,
    size_t lineno)
{
	/* Only the first error is ok */
	if (ms->event_flags & EVENT_HAD_ERR)
		return;
	if (lineno != 0) {
		free(ms->o.buf);
		ms->o.buf = NULL;
		file_printf(ms, "line %" SIZE_T_FORMAT "u: ", lineno);
	}
	file_vprintf(ms, f, va);
	if (error > 0)
		file_printf(ms, " (%s)", strerror(error));
	ms->event_flags |= EVENT_HAD_ERR;
	ms->error = error;
}
