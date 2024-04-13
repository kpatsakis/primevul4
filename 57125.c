int inter_predict_dc(int16_t block[16], int16_t pred[2])
{
    int16_t dc = block[0];
    int ret = 0;

    if (pred[1] > 3) {
        dc += pred[0];
        ret = 1;
    }

    if (!pred[0] | !dc | ((int32_t)pred[0] ^ (int32_t)dc) >> 31) {
        block[0] = pred[0] = dc;
        pred[1] = 0;
    } else {
        if (pred[0] == dc)
            pred[1]++;
        block[0] = pred[0] = dc;
    }

    return ret;
}
