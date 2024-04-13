compare_osa_pw_hist_ent(const void *left_in, const void *right_in)
{
    int kvno_left, kvno_right;
    osa_pw_hist_ent *left = (osa_pw_hist_ent *)left_in;
    osa_pw_hist_ent *right = (osa_pw_hist_ent *)right_in;

    kvno_left = left->n_key_data ? left->key_data[0].key_data_kvno : 0;
    kvno_right = right->n_key_data ? right->key_data[0].key_data_kvno : 0;
    return kvno_left - kvno_right;
}
