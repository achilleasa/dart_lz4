#include <cstring>
#include <cstdio>
#include <new>
#include <dart_api.h>
#include "lz4/lib/lz4.h"
#include "lz4/lib/lz4hc.h"

#define PREFIX_PAD_BYTES 4 // reserve 4 bytes for the uncompressed data length

// Forward declaration of ResolveName function.
Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool *auto_setup_scope);

// The name of the initialization function is the extension name followed
// by _Init.
DART_EXPORT Dart_Handle dart_lz4_Init(Dart_Handle parent_library) {
    if (Dart_IsError(parent_library)) return parent_library;

    Dart_Handle result_code = Dart_SetNativeResolver(parent_library, ResolveName, NULL);
    if (Dart_IsError(result_code)) return result_code;

    return Dart_Null();
}

Dart_Handle HandleError(Dart_Handle handle) {
    if (Dart_IsError(handle)) Dart_PropagateError(handle);
    return handle;
}

void compress_wrapper(Dart_NativeArguments arguments) {
    Dart_EnterScope();

    // Unpack arguments
    Dart_Handle listHandle = HandleError(Dart_GetNativeArgument(arguments, 0));
    if (!Dart_IsTypedData(listHandle) ||
            (Dart_GetTypeOfTypedData(listHandle) != Dart_TypedData_kUint8 && Dart_GetTypeOfExternalTypedData(listHandle) != Dart_TypedData_kUint8)) {
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Data to be compressed should be an instance of Uint8List")));
    }
    bool fastMode = false;
    Dart_Handle boolHandle = HandleError(Dart_GetNativeArgument(arguments, 1));
    if (Dart_IsBoolean(boolHandle)) {
        HandleError(Dart_BooleanValue(boolHandle, &fastMode));
    }

    // Fetch the data to be compressed and its length
    Dart_TypedData_Type type;
    void *uncompressedData;
    intptr_t uncompressedLen;

    Dart_Handle inDataHandle = Dart_TypedDataAcquireData(listHandle, &type, &uncompressedData, &uncompressedLen);
    HandleError(inDataHandle);

    // Allocate buffer for compressed data
    intptr_t minCompressedLen = LZ4_COMPRESSBOUND(uncompressedLen);
    uint8_t *compressedDataBuffer = new(std::nothrow) uint8_t[minCompressedLen + PREFIX_PAD_BYTES];
    if (compressedDataBuffer == NULL) {
        Dart_TypedDataReleaseData(listHandle);
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Could not allocate memory for compressed data buffer")));
    }

    // Compress stream and release input data pointers
    intptr_t copmressedBytes = fastMode
            ? LZ4_compress(reinterpret_cast<char *>(uncompressedData), reinterpret_cast<char *>(compressedDataBuffer + PREFIX_PAD_BYTES), uncompressedLen)
            : LZ4_compressHC(reinterpret_cast<char *>(uncompressedData), reinterpret_cast<char *>(compressedDataBuffer + PREFIX_PAD_BYTES), uncompressedLen);
    Dart_TypedDataReleaseData(listHandle);
    if (copmressedBytes <= 0) {
        delete[] compressedDataBuffer;
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Compression failed")));
    }
    compressedDataBuffer[0] = (uncompressedLen >> 24) & 0xFF;
    compressedDataBuffer[1] = (uncompressedLen >> 16) & 0xFF;
    compressedDataBuffer[2] = (uncompressedLen >> 8) & 0xFF;
    compressedDataBuffer[3] = uncompressedLen & 0xFF;

    // Allocate new Uint8List for compressed data
    intptr_t compressedLen;
    void *compressedData;
    Dart_Handle compressedDataList = Dart_NewTypedData(Dart_TypedData_kUint8, copmressedBytes + PREFIX_PAD_BYTES);
    Dart_Handle outDataHandle = Dart_TypedDataAcquireData(compressedDataList, &type, &compressedData, &compressedLen);

    if (Dart_IsError(compressedDataList) || Dart_IsError(outDataHandle)) {
        delete[] compressedDataBuffer;
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Error allocating memory for compressed data")));
    }

    // Copy data to output list, cleanup tmp buffer and release handles
    memcpy(compressedData, compressedDataBuffer, compressedLen);
    delete[] compressedDataBuffer;
    Dart_TypedDataReleaseData(compressedDataList);
    Dart_SetReturnValue(arguments, compressedDataList);

    Dart_ExitScope();
}

// Native functions get their arguments in a Dart_NativeArguments structure
// and return their results with Dart_SetReturnValue.
void decompress_wrapper(Dart_NativeArguments arguments) {
    Dart_EnterScope();

    // Unpack arguments
    Dart_Handle listHandle = HandleError(Dart_GetNativeArgument(arguments, 0));
    if (!Dart_IsTypedData(listHandle) ||
            (Dart_GetTypeOfTypedData(listHandle) != Dart_TypedData_kUint8 && Dart_GetTypeOfExternalTypedData(listHandle) != Dart_TypedData_kUint8)) {
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Data to be decompressed should be an instance of Uint8List")));
    }

    // Fetch the data to be decompressed and extract length
    Dart_TypedData_Type type;
    uint8_t *compressedData;
    intptr_t compressedLen;

    Dart_Handle dataHandle = Dart_TypedDataAcquireData(listHandle, &type, reinterpret_cast<void **>(&compressedData), &compressedLen);
    HandleError(dataHandle);

    // The first four bytes should indicate the uncompressed payload size
    intptr_t decompressedDataLen = (compressedData[0] << 24) |
            (compressedData[1] << 16) |
            (compressedData[2] << 8) |
            compressedData[3];

    // Allocate buffer for decompression
    uint8_t *decompressedDataBuffer = new(std::nothrow) uint8_t[decompressedDataLen];
    if (decompressedDataBuffer == NULL) {
        Dart_TypedDataReleaseData(listHandle);
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Could not allocate memory for decompressed data buffer")));
    }

    // Decompress data
    int bytesWritten = LZ4_decompress_safe(reinterpret_cast<char *>(compressedData + PREFIX_PAD_BYTES), reinterpret_cast<char *>(decompressedDataBuffer), compressedLen - PREFIX_PAD_BYTES, decompressedDataLen);
    Dart_TypedDataReleaseData(listHandle);
    if (bytesWritten != decompressedDataLen) {
        delete[] decompressedDataBuffer;
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Decompression failed")));
    }

    // Allocate new Uint8List for decompressed data
    intptr_t decompressedLen;
    void *decompressedData;
    Dart_Handle decompressedDataList = Dart_NewTypedData(Dart_TypedData_kUint8, decompressedDataLen);
    Dart_Handle outDataHandle = Dart_TypedDataAcquireData(decompressedDataList, &type, &decompressedData, &decompressedLen);

    if (Dart_IsError(decompressedDataList) || Dart_IsError(outDataHandle)) {
        delete[] decompressedDataBuffer;
        Dart_ExitScope();
        HandleError(Dart_NewUnhandledExceptionError(Dart_NewStringFromCString("Error allocating memory for decompressed data")));
    }

    // Copy data to output list, cleanup tmp buffer and release handles
    memcpy(decompressedData, decompressedDataBuffer, decompressedLen);
    delete[] decompressedDataBuffer;
    Dart_TypedDataReleaseData(decompressedDataList);
    Dart_SetReturnValue(arguments, decompressedDataList);

    Dart_ExitScope();
}


Dart_NativeFunction ResolveName(Dart_Handle name, int argc, bool *auto_setup_scope) {
    // If we fail, we return NULL, and Dart throws an exception.
    if (!Dart_IsString(name)) return NULL;
    Dart_NativeFunction result = NULL;
    const char *cname;
    HandleError(Dart_StringToCString(name, &cname));

    if (strcmp("compress", cname) == 0) result = compress_wrapper;
    else if (strcmp("decompress", cname) == 0) result = decompress_wrapper;
    return result;
}