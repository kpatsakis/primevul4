int crypto_probing_notify(unsigned long val, void *v)
{
	int ok;

	ok = blocking_notifier_call_chain(&crypto_chain, val, v);
	if (ok == NOTIFY_DONE) {
		request_module("cryptomgr");
		ok = blocking_notifier_call_chain(&crypto_chain, val, v);
	}

	return ok;
}
