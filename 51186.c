_archive_filter_count(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct archive_read_filter *p = a->filter;
	int count = 0;
	while(p) {
		count++;
		p = p->upstream;
	}
	return count;
}
