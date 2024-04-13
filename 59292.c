static void swevent_hlist_put_cpu(int cpu)
{
	struct swevent_htable *swhash = &per_cpu(swevent_htable, cpu);

	mutex_lock(&swhash->hlist_mutex);

	if (!--swhash->hlist_refcount)
		swevent_hlist_release(swhash);

	mutex_unlock(&swhash->hlist_mutex);
}
