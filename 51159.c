php_mysqlnd_net_store_length_size(uint64_t length)
{
	if (length < (uint64_t) L64(251)) {
		return 1;
	}
	if (length < (uint64_t) L64(65536)) {
		return 3;
	}
	if (length < (uint64_t) L64(16777216)) {
		return 4;
	}
	return 9;
}
