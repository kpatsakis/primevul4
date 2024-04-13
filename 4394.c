  std::unique_ptr<QuicReadCodec> makeClientEncryptedCodec(
      bool handshakeCipher = false) {
    FizzCryptoFactory cryptoFactory;
    auto readCodec = std::make_unique<QuicReadCodec>(QuicNodeType::Client);
    readCodec->setOneRttReadCipher(test::createNoOpAead());
    readCodec->setOneRttHeaderCipher(test::createNoOpHeaderCipher());
    readCodec->setHandshakeReadCipher(test::createNoOpAead());
    readCodec->setHandshakeHeaderCipher(test::createNoOpHeaderCipher());
    readCodec->setClientConnectionId(*clientConnectionId);
    readCodec->setCodecParameters(
        CodecParameters(kDefaultAckDelayExponent, QuicVersion::MVFST));
    if (handshakeCipher) {
      readCodec->setInitialReadCipher(cryptoFactory.getServerInitialCipher(
          *initialDestinationConnectionId, QuicVersion::MVFST));
      readCodec->setInitialHeaderCipher(
          cryptoFactory.makeServerInitialHeaderCipher(
              *initialDestinationConnectionId, QuicVersion::MVFST));
    }
    return readCodec;
  }