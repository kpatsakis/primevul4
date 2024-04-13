_archive_write_filter_count(struct archive *_a)
{
	struct archive_write *a = (struct archive_write *)_a;
	struct archive_write_filter *p = a->filter_first;
	int count = 0;
	while(p) {
		count++;
		p = p->next_filter;
	}
	return count;
}
