# Dart lz4

[![Build Status](https://drone.io/github.com/achilleasa/dart_lz4/status.png)](https://drone.io/github.com/achilleasa/dart_lz4/latest)

Dart native extension for the [LZ4 codec](https://github.com/Cyan4973/lz4).

# Quick start

```dart
import "dart:typed_data";
import 'package:dart_lz4/dart_lz4.dart' as lz4;

void main() {

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
        print( new String.fromCharCodes(decompressed) );
}
```

# Notes

The LZ4 decompressor needs to be aware of the original uncompressed data length. To ensure that
the codec ```encode()``` output can be piped to the codec ```decode()``` input, the encoder **prepends** the length
of the uncompressed data to the compressed data stream as a **4-byte unsigned int**.

To obtain access to the actual compressed data without reallocating memory you can work with views:

```dart
Uint8List codecOutput = ...
int uncompressedLen = new ByteData.view(codecOutput.buffer, 0, 4).getUInt32(0);
Uint8List compressedData = new Uint8List.view(codecOutput.buffer, 4, codecOutput.lengthInBytes - 4);
```

# Building the extension

To build the extension you need to install [cmake](http://www.cmake.org/). If you have installed the
dart-sdk at a non-standard location (e.g. you home directory) you need to set the ```DART_SDK``` env
var to point to the dart-sdk folder. The extension can be built as follows:

## Linux and Mac
```
cmake .
make
```

## Windows

Install [Microsoft Windows SDK for Windows 7](http://www.microsoft.com/en-us/download/details.aspx?id=8279)

```
cmake . -G"Visual Studio 10 2010 Win64"
cmake --build . --config Release
```

# Using precompiled 64-bit binaries

Precompiled 64-bit binaries for linux, mac and windows can be obtained by downloading
the latest binary [release](https://github.com/achilleasa/dart_lz4/releases/latest) from github or by checking out the [master-binaries](https://github.com/achilleasa/dart_lz4/tree/master-binaries) branch.

You may also directly checkout the master-binaries branch by adding the following to your
```pubspec.yaml``` file:

```yaml
dependencies:
  dart_lz4:
    git:
      url: git://github.com/achilleasa/dart_lz4.git
      ref: master-binaries
```

# Contributing

See the [Contributing Guide](https://github.com/achilleasa/dart_lz4/blob/master/CONTRIBUTING.md).


# License

dart\_lz4 is distributed under the [MIT license](https://github.com/achilleasa/dart_lz4/blob/master/LICENSE).
