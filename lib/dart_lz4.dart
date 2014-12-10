library dart_lz4;

import "dart:typed_data";
import "dart-ext:dart_lz4";
import "dart:convert";

// Define native extension functions
Uint8List _compress(Uint8List data, [ bool fast = true ]) native "compress";

Uint8List _decompress(Uint8List data) native "decompress";

class _LZ4Encoder extends Converter<Uint8List, Uint8List> {

  final bool _fast;

  const _LZ4Encoder(this._fast);

  Uint8List convert(Uint8List input) => _compress(input, _fast);
}

class _LZ4Decoder extends Converter<Uint8List, Uint8List> {

  Uint8List convert(Uint8List input) => _decompress(input);

  const _LZ4Decoder();
}

class CompressionMode {

  final int _value;

  static const CompressionMode FAST = const CompressionMode._(0x01);
  static const CompressionMode HIGH = const CompressionMode._(0x02);

  const CompressionMode._(int this._value);
}

class Lz4Codec extends Codec<Uint8List, Uint8List> {

  Converter<Uint8List, Uint8List> _encoder;

  final Converter<Uint8List, Uint8List> _decoder = const _LZ4Decoder();

  Lz4Codec({CompressionMode mode : CompressionMode.FAST}) : super() {
    _encoder = mode == CompressionMode.FAST
               ? const _LZ4Encoder(true)
               : const _LZ4Encoder(false);
  }

  Converter<Uint8List, Uint8List> get encoder => _encoder;

  Converter<Uint8List, Uint8List> get decoder => _decoder;
}