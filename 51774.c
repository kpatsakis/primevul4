get_tb_count(struct fp_info *p_fp_info)
{
    int chan, tb_count = 0;
    for (chan = 0; chan < p_fp_info->num_chans; chan++) {
        tb_count += p_fp_info->chan_num_tbs[chan];
    }
    return tb_count;
}
