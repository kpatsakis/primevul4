static int dnxhd_get_hr_frame_size(int cid, int w, int h)
{
    int result, i = ff_dnxhd_get_cid_table(cid);

    if (i < 0)
        return i;

    result = ((h + 15) / 16) * ((w + 15) / 16) * ff_dnxhd_cid_table[i].packet_scale.num / ff_dnxhd_cid_table[i].packet_scale.den;
    result = (result + 2048) / 4096 * 4096;

    return FFMAX(result, 8192);
}
