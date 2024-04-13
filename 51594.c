dump_isodirrec(FILE *out, const unsigned char *isodirrec)
{
	fprintf(out, " l %d,",
	    toi(isodirrec + DR_length_offset, DR_length_size));
	fprintf(out, " a %d,",
	    toi(isodirrec + DR_ext_attr_length_offset, DR_ext_attr_length_size));
	fprintf(out, " ext 0x%x,",
	    toi(isodirrec + DR_extent_offset, DR_extent_size));
	fprintf(out, " s %d,",
	    toi(isodirrec + DR_size_offset, DR_extent_size));
	fprintf(out, " f 0x%x,",
	    toi(isodirrec + DR_flags_offset, DR_flags_size));
	fprintf(out, " u %d,",
	    toi(isodirrec + DR_file_unit_size_offset, DR_file_unit_size_size));
	fprintf(out, " ilv %d,",
	    toi(isodirrec + DR_interleave_offset, DR_interleave_size));
	fprintf(out, " seq %d,",
	    toi(isodirrec + DR_volume_sequence_number_offset,
		DR_volume_sequence_number_size));
	fprintf(out, " nl %d:",
	    toi(isodirrec + DR_name_len_offset, DR_name_len_size));
	fprintf(out, " `%.*s'",
	    toi(isodirrec + DR_name_len_offset, DR_name_len_size),
		isodirrec + DR_name_offset);
}
