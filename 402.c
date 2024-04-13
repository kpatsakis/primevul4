v8::Handle<v8::Value> V8ThrowException::throwReferenceError(v8::Isolate* isolate, const String& message)
{
    v8::Handle<v8::Value> exception = V8ThrowException::createReferenceError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}
