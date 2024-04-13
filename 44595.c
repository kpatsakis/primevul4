static int wb_priority(struct writeback_control *wbc)
{
	if (wbc->for_reclaim)
		return FLUSH_HIGHPRI | FLUSH_STABLE;
	if (wbc->for_kupdate || wbc->for_background)
		return FLUSH_LOWPRI | FLUSH_COND_STABLE;
	return FLUSH_COND_STABLE;
}
