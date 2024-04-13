static struct segment *current_segment(struct playlist *pls)
{
    return pls->segments[pls->cur_seq_no - pls->start_seq_no];
}
