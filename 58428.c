static void insert_ts(AVBPrint *buf, int ts)
{
    if (ts == -1) {
        av_bprintf(buf, "9:59:59.99,");
    } else {
        int h, m, s;

        h = ts/360000;  ts -= 360000*h;
        m = ts/  6000;  ts -=   6000*m;
        s = ts/   100;  ts -=    100*s;
        av_bprintf(buf, "%d:%02d:%02d.%02d,", h, m, s, ts);
    }
}
