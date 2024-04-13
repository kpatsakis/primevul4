njs_array_alloc(njs_vm_t *vm, njs_bool_t flat, uint64_t length, uint32_t spare)
{
    uint64_t     size;
    njs_int_t    ret;
    njs_array_t  *array;
    njs_value_t  value;

    if (njs_slow_path(length > UINT32_MAX)) {
        goto overflow;
    }

    array = njs_mp_alloc(vm->mem_pool, sizeof(njs_array_t));
    if (njs_slow_path(array == NULL)) {
        goto memory_error;
    }

    size = length + spare;

    if (flat || size <= NJS_ARRAY_LARGE_OBJECT_LENGTH) {
        array->data = njs_mp_align(vm->mem_pool, sizeof(njs_value_t),
                                   size * sizeof(njs_value_t));
        if (njs_slow_path(array->data == NULL)) {
            goto memory_error;
        }

    } else {
        array->data = NULL;
    }

    array->start = array->data;
    njs_lvlhsh_init(&array->object.hash);
    array->object.shared_hash = vm->shared->array_instance_hash;
    array->object.__proto__ = &vm->prototypes[NJS_OBJ_TYPE_ARRAY].object;
    array->object.slots = NULL;
    array->object.type = NJS_ARRAY;
    array->object.shared = 0;
    array->object.extensible = 1;
    array->object.error_data = 0;
    array->object.fast_array = (array->data != NULL);

    if (njs_fast_path(array->object.fast_array)) {
        array->size = size;
        array->length = length;

    } else {
        array->size = 0;
        array->length = 0;

        njs_set_array(&value, array);
        ret = njs_array_length_redefine(vm, &value, length);
        if (njs_slow_path(ret != NJS_OK)) {
            return NULL;
        }
    }

    return array;

memory_error:

    njs_memory_error(vm);

    return NULL;

overflow:

    njs_range_error(vm, "Invalid array length");

    return NULL;
}