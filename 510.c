v8::Handle<v8::Value> V8ThrowException::throwException(v8::Handle<v8::Value> exception, v8::Isolate* isolate)
{
    if (!v8::V8::IsExecutionTerminating())
        isolate->ThrowException(exception);
    return v8::Undefined(isolate);
}
