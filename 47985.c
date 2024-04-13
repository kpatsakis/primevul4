local long tolong(unsigned long val)
{
    return (long)(val & 0x7fffffffUL) - (long)(val & 0x80000000UL);
}
