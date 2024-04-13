static int has_prec(int val)
{
    if (val < 2) {
        return 1;
    }
    if (val < 4) {
        return 2;
    }
    if (val < 8) {
        return 3;
    }
    if (val < 16) {
        return 4;
    }
    if (val < 32) {
        return 5;
    }
    if (val < 64) {
        return 6;
    }
    if (val < 128) {
        return 7;
    }
    if (val < 256) {
        return 8;
    }
    if (val < 512) {
        return 9;
    }
    if (val < 1024) {
        return 10;
    }
    if (val < 2048) {
        return 11;
    }
    if (val < 4096) {
        return 12;
    }
    if (val < 8192) {
        return 13;
    }
    if (val < 16384) {
        return 14;
    }
    if (val < 32768) {
        return 15;
    }
    return 16;
}
