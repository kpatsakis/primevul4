VOID CALLBACK tq_timer_cb(PVOID arg, BOOLEAN timed_out)
{
	zend_bool *php_timed_out;

	/* The doc states it'll be always true, however it theoretically
		could be FALSE when the thread was signaled. */
	if (!timed_out) {
		return;
	}

	php_timed_out = (zend_bool *)arg;
	*php_timed_out = 1;
}
