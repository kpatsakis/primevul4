njs_array_compare(const void *a, const void *b, void *c)
{
    double                 num;
    njs_int_t              ret;
    njs_value_t            arguments[3], retval;
    njs_array_sort_ctx_t   *ctx;
    njs_array_sort_slot_t  *aslot, *bslot;

    ctx = c;

    if (ctx->exception) {
        return 0;
    }

    aslot = (njs_array_sort_slot_t *) a;
    bslot = (njs_array_sort_slot_t *) b;

    if (ctx->function != NULL) {
        njs_set_undefined(&arguments[0]);
        arguments[1] = aslot->value;
        arguments[2] = bslot->value;

        ret = njs_function_apply(ctx->vm, ctx->function, arguments, 3, &retval);
        if (njs_slow_path(ret != NJS_OK)) {
            goto exception;
        }

        ret = njs_value_to_number(ctx->vm, &retval, &num);
        if (njs_slow_path(ret != NJS_OK)) {
            goto exception;
        }

        if (njs_slow_path(isnan(num))) {
            return 0;
        }

        if (num != 0) {
            return (num > 0) - (num < 0);
        }

        goto compare_same;
    }

    if (aslot->str == NULL) {
        aslot->str = njs_arr_add(&ctx->strings);
        ret = njs_value_to_string(ctx->vm, aslot->str, &aslot->value);
        if (njs_slow_path(ret != NJS_OK)) {
            goto exception;
        }
    }

    if (bslot->str == NULL) {
        bslot->str = njs_arr_add(&ctx->strings);
        ret = njs_value_to_string(ctx->vm, bslot->str, &bslot->value);
        if (njs_slow_path(ret != NJS_OK)) {
            goto exception;
        }
    }

    ret = njs_string_cmp(aslot->str, bslot->str);

    if (ret != 0) {
        return ret;
    }

compare_same:

    /* Ensures stable sorting. */

    return (aslot->pos > bslot->pos) - (aslot->pos < bslot->pos);

exception:

    ctx->exception = 1;

    return 0;
}