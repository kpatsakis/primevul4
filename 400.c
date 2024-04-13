static void domExceptionStackSetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    ASSERT(info.Data()->IsObject());
    info.Data()->ToObject(info.GetIsolate())->Set(v8AtomicString(info.GetIsolate(), "stack"), value);
}
