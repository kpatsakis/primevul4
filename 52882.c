static unsigned char convert_num_notes_to_fanout(uintmax_t num_notes)
{
	unsigned char fanout = 0;
	while ((num_notes >>= 8))
		fanout++;
	return fanout;
}
