v8::Handle<v8::Value> V8ThrowException::createGeneralError(v8::Isolate* isolate, const String& message)
{
    return v8::Exception::Error(v8String(isolate, message.isNull() ? "Error" : message));
}
