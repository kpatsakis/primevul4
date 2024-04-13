static int color_table_compare(const void *lhs, const void *rhs)
{
    return av_strcasecmp(lhs, ((const ColorEntry *)rhs)->name);
}
