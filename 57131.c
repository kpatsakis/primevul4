static int setup_partitions(VP8Context *s, const uint8_t *buf, int buf_size)
{
    const uint8_t *sizes = buf;
    int i;
    int ret;

    s->num_coeff_partitions = 1 << vp8_rac_get_uint(&s->c, 2);

    buf      += 3 * (s->num_coeff_partitions - 1);
    buf_size -= 3 * (s->num_coeff_partitions - 1);
    if (buf_size < 0)
        return -1;

    for (i = 0; i < s->num_coeff_partitions - 1; i++) {
        int size = AV_RL24(sizes + 3 * i);
        if (buf_size - size < 0)
            return -1;

        ret = ff_vp56_init_range_decoder(&s->coeff_partition[i], buf, size);
        if (ret < 0)
            return ret;
        buf      += size;
        buf_size -= size;
    }
    return ff_vp56_init_range_decoder(&s->coeff_partition[i], buf, buf_size);
}
