njs_function_call2(njs_vm_t *vm, njs_function_t *function,
    const njs_value_t *this, const njs_value_t *args,
    njs_uint_t nargs, njs_value_t *retval, njs_bool_t ctor)
{
    njs_int_t    ret;
    njs_value_t  dst njs_aligned(16);

    ret = njs_function_frame(vm, function, this, args, nargs, ctor);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_function_frame_invoke(vm, &dst);

    if (ret == NJS_OK) {
        *retval = dst;
    }

    return ret;
}