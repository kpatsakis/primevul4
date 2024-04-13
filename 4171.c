njs_array_indices_handler(const void *first, const void *second, void *ctx)
{
    double             num1, num2;
    int64_t            diff;
    njs_str_t          str1, str2;
    const njs_value_t  *val1, *val2;

    val1 = first;
    val2 = second;

    num1 = njs_string_to_index(val1);
    num2 = njs_string_to_index(val2);

    if (!isnan(num1) || !isnan(num2)) {
        if (isnan(num1)) {
            return 1;
        }

        if (isnan(num2)) {
            return -1;
        }

        diff = (int64_t) (num1 - num2);

        if (diff < 0) {
            return -1;
        }

        return diff != 0;
    }

    njs_string_get(val1, &str1);
    njs_string_get(val2, &str2);

    return strncmp((const char *) str1.start, (const char *) str2.start,
                   njs_min(str1.length, str2.length));
}