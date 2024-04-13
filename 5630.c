get_unname_register()
{
    return y_previous == NULL ? -1 : y_previous - &y_regs[0];
}