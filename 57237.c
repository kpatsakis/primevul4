static unsigned hex_char_to_number(uint8_t x)
{
    if (x >= 'a' && x <= 'f')
        x -= 'a' - 10;
    else if (x >= 'A' && x <= 'F')
        x -= 'A' - 10;
    else if (x >= '0' && x <= '9')
        x -= '0';
    else
        x = 0;
    return x;
}
