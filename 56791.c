static inline void update_rice(APERice *rice, unsigned int x)
{
    int lim = rice->k ? (1 << (rice->k + 4)) : 0;
    rice->ksum += ((x + 1) / 2) - ((rice->ksum + 16) >> 5);

    if (rice->ksum < lim)
        rice->k--;
    else if (rice->ksum >= (1 << (rice->k + 5)))
        rice->k++;
}
