v8::Handle<v8::Value> V8ThrowException::throwSyntaxError(v8::Isolate* isolate, const String& message)
{
    v8::Handle<v8::Value> exception = V8ThrowException::createSyntaxError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}
