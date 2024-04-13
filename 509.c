v8::Handle<v8::Value> V8ThrowException::createReferenceError(v8::Isolate* isolate, const String& message)
{
    return v8::Exception::ReferenceError(v8String(isolate, message.isNull() ? "Reference error" : message));
}
