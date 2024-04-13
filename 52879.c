static void checkpoint(void)
{
	checkpoint_requested = 0;
	if (object_count) {
		cycle_packfile();
		dump_branches();
		dump_tags();
		dump_marks();
	}
}
