v8::Handle<v8::Value> V8ThrowException::throwTypeError(v8::Isolate* isolate, const String& message)
{
    v8::Handle<v8::Value> exception = V8ThrowException::createTypeError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}
