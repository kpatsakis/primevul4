static void vp78_reset_probability_tables(VP8Context *s)
{
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 16; j++)
            memcpy(s->prob->token[i][j], vp8_token_default_probs[i][vp8_coeff_band[j]],
                   sizeof(s->prob->token[i][j]));
}
