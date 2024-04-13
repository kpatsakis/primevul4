static INLINE int clamp(const int value, const int prec, const int sgnd)
{
    if (sgnd) {
        if (prec <= 8) {
            return CLAMP(value, -128, 127);
        } else if (prec <= 16) {
            return CLAMP(value, -32768, 32767);
        } else {
            return CLAMP(value, -2147483647 - 1, 2147483647);
        }
    } else {
        if (prec <= 8) {
            return CLAMP(value, 0, 255);
        } else if (prec <= 16) {
            return CLAMP(value, 0, 65535);
        } else {
            return value;    /*CLAMP(value,0,4294967295);*/
        }
    }
}
