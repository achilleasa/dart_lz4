library dart_lz4.tests;

import "dart:io";
import "dart:typed_data";
import "package:unittest/unittest.dart";
import '../lib/dart_lz4.dart' as lz4;

void main(List<String> args) {

  lz4.Lz4Codec fastCodec = new lz4.Lz4Codec(mode : lz4.CompressionMode.FAST);
  lz4.Lz4Codec hcCodec = new lz4.Lz4Codec(mode : lz4.CompressionMode.HIGH);

  group("LZ4:", () {

    test("compress without an instance of Uint8List as input should throw", () {
      return new File("lz4_test.dart").readAsBytes().then((List<int> data) {
        dynamic _data = new Uint16List.fromList(data);
        expect(() => fastCodec.encode(_data)
        , throwsA((e) => e == "Data to be compressed should be an instance of Uint8List")
        );
      });
    });

    test("compress (fast) -> decompress", () {
      return new File("lz4_test.dart").readAsBytes().then((List<int> data) {
        Uint8List compressed = fastCodec.encode(data);
        expect(compressed, new isInstanceOf<Uint8List>());
        Uint8List decompressed = fastCodec.decode(compressed);
        expect(decompressed, new isInstanceOf<Uint8List>());
      });
    });

    test("compress (hc) -> decompress", () {
      return new File("lz4_test.dart").readAsBytes().then((List<int> data) {
        Uint8List compressed = hcCodec.encode(data);
        expect(compressed, new isInstanceOf<Uint8List>());
        Uint8List decompressed = hcCodec.decode(compressed);
        expect(decompressed, new isInstanceOf<Uint8List>());
      });
    });

    test("compress and decompress string", () {
      // Alternatively you can specify lz4.CompressionMode.HIGH for higher
      // performance but less speed.
      lz4.Lz4Codec codec = new lz4.Lz4Codec(mode : lz4.CompressionMode.FAST);

      String test = '''Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor
                     incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud
                     exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute
                     irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat
                     nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui
                     officia deserunt mollit anim id est laborum.''';

      // The compressor will *prepend* the original length to the compressed stream output
      Uint8List compressed = codec.encode(new Uint8List.fromList(test.codeUnits));
      Uint8List decompressed = codec.decode(compressed);
      expect(new String.fromCharCodes(decompressed), equals(test));
    });
  });
}