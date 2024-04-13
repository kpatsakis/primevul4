ampdu_reassemble_cleanup(void)
{
    reassembly_table_destroy(&ampdu_reassembly_table);
}
