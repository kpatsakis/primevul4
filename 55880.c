static int int_floorlog2(int a)
{
    int l;
    for (l = 0; a > 1; l++) {
        a >>= 1;
    }
    return l;
}
