static void parse_new_blob(void)
{
	read_next_command();
	parse_mark();
	parse_and_store_blob(&last_blob, NULL, next_mark);
}
