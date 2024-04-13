iperf_on_test_start(struct iperf_test *test)
{
    if (test->json_output) {
	cJSON_AddItemToObject(test->json_start, "test_start", iperf_json_printf("protocol: %s  num_streams: %d  blksize: %d  omit: %d  duration: %d  bytes: %d  blocks: %d  reverse: %d", test->protocol->name, (int64_t) test->num_streams, (int64_t) test->settings->blksize, (int64_t) test->omit, (int64_t) test->duration, (int64_t) test->settings->bytes, (int64_t) test->settings->blocks, test->reverse?(int64_t)1:(int64_t)0));
    } else {
	if (test->verbose) {
	    if (test->settings->bytes)
		iprintf(test, test_start_bytes, test->protocol->name, test->num_streams, test->settings->blksize, test->omit, test->settings->bytes);
	    else if (test->settings->blocks)
		iprintf(test, test_start_blocks, test->protocol->name, test->num_streams, test->settings->blksize, test->omit, test->settings->blocks);
	    else
		iprintf(test, test_start_time, test->protocol->name, test->num_streams, test->settings->blksize, test->omit, test->duration);
	}
    }
}
