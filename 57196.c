static int read_huffman_code_normal(WebPContext *s, HuffReader *hc,
                                    int alphabet_size)
{
    HuffReader code_len_hc = { { 0 }, 0, 0, { 0 } };
    int *code_lengths = NULL;
    int code_length_code_lengths[NUM_CODE_LENGTH_CODES] = { 0 };
    int i, symbol, max_symbol, prev_code_len, ret;
    int num_codes = 4 + get_bits(&s->gb, 4);

    if (num_codes > NUM_CODE_LENGTH_CODES)
        return AVERROR_INVALIDDATA;

    for (i = 0; i < num_codes; i++)
        code_length_code_lengths[code_length_code_order[i]] = get_bits(&s->gb, 3);

    ret = huff_reader_build_canonical(&code_len_hc, code_length_code_lengths,
                                      NUM_CODE_LENGTH_CODES);
    if (ret < 0)
        goto finish;

    code_lengths = av_mallocz_array(alphabet_size, sizeof(*code_lengths));
    if (!code_lengths) {
        ret = AVERROR(ENOMEM);
        goto finish;
    }

    if (get_bits1(&s->gb)) {
        int bits   = 2 + 2 * get_bits(&s->gb, 3);
        max_symbol = 2 + get_bits(&s->gb, bits);
        if (max_symbol > alphabet_size) {
            av_log(s->avctx, AV_LOG_ERROR, "max symbol %d > alphabet size %d\n",
                   max_symbol, alphabet_size);
            ret = AVERROR_INVALIDDATA;
            goto finish;
        }
    } else {
        max_symbol = alphabet_size;
    }

    prev_code_len = 8;
    symbol        = 0;
    while (symbol < alphabet_size) {
        int code_len;

        if (!max_symbol--)
            break;
        code_len = huff_reader_get_symbol(&code_len_hc, &s->gb);
        if (code_len < 16) {
            /* Code length code [0..15] indicates literal code lengths. */
            code_lengths[symbol++] = code_len;
            if (code_len)
                prev_code_len = code_len;
        } else {
            int repeat = 0, length = 0;
            switch (code_len) {
            case 16:
                /* Code 16 repeats the previous non-zero value [3..6] times,
                 * i.e., 3 + ReadBits(2) times. If code 16 is used before a
                 * non-zero value has been emitted, a value of 8 is repeated. */
                repeat = 3 + get_bits(&s->gb, 2);
                length = prev_code_len;
                break;
            case 17:
                /* Code 17 emits a streak of zeros [3..10], i.e.,
                 * 3 + ReadBits(3) times. */
                repeat = 3 + get_bits(&s->gb, 3);
                break;
            case 18:
                /* Code 18 emits a streak of zeros of length [11..138], i.e.,
                 * 11 + ReadBits(7) times. */
                repeat = 11 + get_bits(&s->gb, 7);
                break;
            }
            if (symbol + repeat > alphabet_size) {
                av_log(s->avctx, AV_LOG_ERROR,
                       "invalid symbol %d + repeat %d > alphabet size %d\n",
                       symbol, repeat, alphabet_size);
                ret = AVERROR_INVALIDDATA;
                goto finish;
            }
            while (repeat-- > 0)
                code_lengths[symbol++] = length;
        }
    }

    ret = huff_reader_build_canonical(hc, code_lengths, alphabet_size);

finish:
    ff_free_vlc(&code_len_hc.vlc);
    av_free(code_lengths);
    return ret;
}
