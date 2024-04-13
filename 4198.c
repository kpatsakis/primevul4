njs_array_prototype_iterator_obj(njs_vm_t *vm, njs_value_t *args,
    njs_uint_t nargs, njs_index_t kind)
{
    njs_int_t    ret;
    njs_value_t  *this;

    this = njs_argument(args, 0);

    ret = njs_value_to_object(vm, this);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    return njs_array_iterator_create(vm, this, &vm->retval, kind);
}