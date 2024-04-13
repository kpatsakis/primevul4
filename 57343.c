mrb_objspace_each_objects(mrb_state *mrb, mrb_each_object_callback *callback, void *data)
{
  gc_each_objects(mrb, &mrb->gc, callback, data);
}
