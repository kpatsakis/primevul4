void perf_swevent_put_recursion_context(int rctx)
{
	struct swevent_htable *swhash = this_cpu_ptr(&swevent_htable);

	put_recursion_context(swhash->recursion, rctx);
}
