void digest_algo_prints(const char *prefix)
{
	struct digest_algo* d;

	printf("%s%-15s\t%-20s\t%-15s\n", prefix, "name", "driver", "priority");
	printf("%s--------------------------------------------------\n", prefix);
	list_for_each_entry(d, &digests, list) {
		printf("%s%-15s\t%-20s\t%d\n", prefix, d->base.name,
			d->base.driver_name, d->base.priority);
	}
}