ZEND_API void zend_timeout(int dummy) /* {{{ */
{

	if (zend_on_timeout) {
#ifdef ZEND_SIGNALS
		/*
		   We got here because we got a timeout signal, so we are in a signal handler
		   at this point. However, we want to be able to timeout any user-supplied
		   shutdown functions, so pretend we are not in a signal handler while we are
		   calling these
		*/
		SIGG(running) = 0;
#endif
		zend_on_timeout(EG(timeout_seconds));
	}

	zend_error_noreturn(E_ERROR, "Maximum execution time of %pd second%s exceeded", EG(timeout_seconds), EG(timeout_seconds) == 1 ? "" : "s");
}
/* }}} */
