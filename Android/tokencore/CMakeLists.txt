cmake_minimum_required(VERSION 3.4.1)

ADD_DEFINITIONS(
        -DHAVE_PTHREAD
#        -DuECC_ENABLE_VLI_API
)

set(TOKENCORE_COMMON_SRCS
        #=============================================
        # Common
        #=============================================
        ../../TokenCore/u256.h
        ../../TokenCore/u256.cpp
        ../../TokenCore/TokenCommon.cpp

        #=============================================
        # uECC
        #=============================================
        ../../TokenCore/uEcc/curve-specific.inc
        ../../TokenCore/uEcc/macroECC.c
        ../../TokenCore/uEcc/macroECC.h
        ../../TokenCore/uEcc/platform-specific.inc
        ../../TokenCore/uEcc/types.h
        ../../TokenCore/uEcc/uECC.h
        ../../TokenCore/uEcc/uECC.c
        ../../TokenCore/uEcc/uECC_vli.h

        #=============================================
        # Crypto
        #=============================================
        ../../TokenCore/crypto/base58.h
        ../../TokenCore/crypto/base58.cpp
        ../../TokenCore/crypto/bip.h
        ../../TokenCore/crypto/bip.cpp
        ../../TokenCore/crypto/bx_plat.h
        ../../TokenCore/crypto/bx_plat.cpp
        ../../TokenCore/crypto/bx_str.h
        ../../TokenCore/crypto/bx_str.cpp
        ../../TokenCore/crypto/hash.h
        ../../TokenCore/crypto/hash.cpp
        ../../TokenCore/crypto/hmac_sha512.h
        ../../TokenCore/crypto/hmac_sha512.cpp
        ../../TokenCore/crypto/keccak256.h
        ../../TokenCore/crypto/keccak256.c
        ../../TokenCore/crypto/ripemd160.h
        ../../TokenCore/crypto/ripemd160.cpp
        ../../TokenCore/crypto/sha1.h
        ../../TokenCore/crypto/sha1.cpp
        ../../TokenCore/crypto/sha2.h
        ../../TokenCore/crypto/sha2.cpp
        ../../TokenCore/crypto/sha256.h
        ../../TokenCore/crypto/sha256.cpp
        ../../TokenCore/crypto/sha512.h
        ../../TokenCore/crypto/sha512.cpp
        ../../TokenCore/crypto/tx_util.h
        ../../TokenCore/crypto/tx_util.cpp
        ../../TokenCore/crypto/util.h
        ../../TokenCore/crypto/util.c
        ../../TokenCore/crypto/utility_tools.h
        ../../TokenCore/crypto/utility_tools.cpp
        ../../TokenCore/crypto/pbkdf2.h
        ../../TokenCore/crypto/pbkdf2.cpp
        ../../TokenCore/crypto/hmac.h
        ../../TokenCore/crypto/hmac.cpp

        #=============================================
        # mbedTLS
        #=============================================
        ../../TokenCore/mbedtls/include/mbedtls/config.h
        ../../TokenCore/mbedtls/include/mbedtls/bignum.h
        ../../TokenCore/mbedtls/library/bignum.c
        ../../TokenCore/mbedtls/include/mbedtls/ecp.h
        ../../TokenCore/mbedtls/library/ecp.c
        ../../TokenCore/mbedtls/library/ecp_curves.c

        #=============================================
        # JsonCPP
        #=============================================
        ../../TokenCore/jsoncpp/include/json/assertions.h
        ../../TokenCore/jsoncpp/include/json/autolink.h
        ../../TokenCore/jsoncpp/include/json/config.h
        ../../TokenCore/jsoncpp/include/json/features.h
        ../../TokenCore/jsoncpp/include/json/forwards.h
        ../../TokenCore/jsoncpp/include/json/json.h
        ../../TokenCore/jsoncpp/include/json/reader.h
        ../../TokenCore/jsoncpp/include/json/value.h
        ../../TokenCore/jsoncpp/include/json/version.h
        ../../TokenCore/jsoncpp/include/json/writer.h
        ../../TokenCore/jsoncpp/src/lib_json/json_reader.cpp
        ../../TokenCore/jsoncpp/src/lib_json/json_tool.h
        ../../TokenCore/jsoncpp/src/lib_json/json_value.cpp
        ../../TokenCore/jsoncpp/src/lib_json/json_valueiterator.inl
        ../../TokenCore/jsoncpp/src/lib_json/json_writer.cpp

        #=============================================
        # ChainKit, for EOS
        #=============================================
        ../../TokenCore/Eos/action.cpp
        ../../TokenCore/Eos/action.h
        ../../TokenCore/Eos/chainbase.h
        ../../TokenCore/Eos/chainmanager.cpp
        ../../TokenCore/Eos/chainmanager.h
        ../../TokenCore/Eos/eos_key_encode.cpp
        ../../TokenCore/Eos/eos_key_encode.h
        ../../TokenCore/Eos/eosapi_utils.h
        ../../TokenCore/Eos/eosapi_utils.cpp
        ../../TokenCore/Eos/eosbytewriter.cpp
        ../../TokenCore/Eos/eosbytewriter.h
        ../../TokenCore/Eos/eosnewaccount.cpp
        ../../TokenCore/Eos/eosnewaccount.h
        ../../TokenCore/Eos/packedtransaction.cpp
        ../../TokenCore/Eos/packedtransaction.h
        ../../TokenCore/Eos/signedtransaction.cpp
        ../../TokenCore/Eos/signedtransaction.h
        ../../TokenCore/Eos/transaction.cpp
        ../../TokenCore/Eos/transaction.h
        ../../TokenCore/Eos/transactionextension.cpp
        ../../TokenCore/Eos/transactionextension.h
        ../../TokenCore/Eos/transactionheader.cpp
        ../../TokenCore/Eos/transactionheader.h
        ../../TokenCore/Eos/typeaccountpermissionweight.cpp
        ../../TokenCore/Eos/typeaccountpermissionweight.h
        ../../TokenCore/Eos/typeauthority.cpp
        ../../TokenCore/Eos/typeauthority.h
        ../../TokenCore/Eos/typekeypermissionweight.cpp
        ../../TokenCore/Eos/typekeypermissionweight.h
        ../../TokenCore/Eos/typename.cpp
        ../../TokenCore/Eos/typename.h
        ../../TokenCore/Eos/typepermissionlevel.cpp
        ../../TokenCore/Eos/typepermissionlevel.h
        ../../TokenCore/Eos/typewaitweight.cpp
        ../../TokenCore/Eos/typewaitweight.h
#        ../../TokenCore/chainkit/Crypto/libbase58.h
#        ../../TokenCore/chainkit/Crypto/libbase58.c
#        ../../TokenCore/chainkit/Crypto/options.h
#        ../../TokenCore/chainkit/Crypto/rmd160.h
#        ../../TokenCore/chainkit/Crypto/rmd160.c
#        ../../TokenCore/chainkit/Crypto/sha2.c
#        ../../TokenCore/chainkit/Crypto/sha2.h
#        ../../TokenCore/chainkit/Crypto/sha3.c
#        ../../TokenCore/chainkit/Crypto/sha3.h
#        ../../TokenCore/chainkit/Crypto/macroECC.c
#        ../../TokenCore/chainkit/Crypto/macroECC.h
#        ../../TokenCore/chainkit/ec/eos_key_encode.cpp
#        ../../TokenCore/chainkit/ec/eos_key_encode.h
#        ../../TokenCore/chainkit/ec/c_sha256.cpp
#        ../../TokenCore/chainkit/ec/c_sha256.h
#        ../../TokenCore/chainkit/ec/typechainid.cpp
#        ../../TokenCore/chainkit/ec/typechainid.h
##        ../../TokenCore/chainkit/utility/utils.h
##        ../../TokenCore/chainkit/utility/utils.cpp

        #=============================================
        # google protobuf, for TRX ?????????
        #=============================================
        ../../TokenCore/Tron/google/protobuf/any.cc
        ../../TokenCore/Tron/google/protobuf/any.h
        ../../TokenCore/Tron/google/protobuf/any.pb.cc
        ../../TokenCore/Tron/google/protobuf/any.pb.h
        ../../TokenCore/Tron/google/protobuf/api.pb.cc
        ../../TokenCore/Tron/google/protobuf/api.pb.h
        ../../TokenCore/Tron/google/protobuf/arena.cc
        ../../TokenCore/Tron/google/protobuf/arena.h
        ../../TokenCore/Tron/google/protobuf/arenastring.cc
        ../../TokenCore/Tron/google/protobuf/arenastring.h
        ../../TokenCore/Tron/google/protobuf/arena_impl.h
        ../../TokenCore/Tron/google/protobuf/descriptor.cc
        ../../TokenCore/Tron/google/protobuf/descriptor.h
        ../../TokenCore/Tron/google/protobuf/descriptor.pb.cc
        ../../TokenCore/Tron/google/protobuf/descriptor.pb.h
        ../../TokenCore/Tron/google/protobuf/descriptor_database.cc
        ../../TokenCore/Tron/google/protobuf/descriptor_database.h
        ../../TokenCore/Tron/google/protobuf/duration.pb.cc
        ../../TokenCore/Tron/google/protobuf/duration.pb.h
        ../../TokenCore/Tron/google/protobuf/dynamic_message.cc
        ../../TokenCore/Tron/google/protobuf/dynamic_message.h
        ../../TokenCore/Tron/google/protobuf/empty.pb.cc
        ../../TokenCore/Tron/google/protobuf/empty.pb.h
        ../../TokenCore/Tron/google/protobuf/extension_set.cc
        ../../TokenCore/Tron/google/protobuf/extension_set.h
        ../../TokenCore/Tron/google/protobuf/extension_set_heavy.cc
        ../../TokenCore/Tron/google/protobuf/field_mask.pb.cc
        ../../TokenCore/Tron/google/protobuf/field_mask.pb.h
        ../../TokenCore/Tron/google/protobuf/generated_enum_reflection.h
        ../../TokenCore/Tron/google/protobuf/generated_enum_util.h
        ../../TokenCore/Tron/google/protobuf/generated_message_reflection.cc
        ../../TokenCore/Tron/google/protobuf/generated_message_reflection.h
        ../../TokenCore/Tron/google/protobuf/generated_message_table_driven.cc
        ../../TokenCore/Tron/google/protobuf/generated_message_table_driven.h
        ../../TokenCore/Tron/google/protobuf/generated_message_table_driven_lite.cc
        ../../TokenCore/Tron/google/protobuf/generated_message_table_driven_lite.h
        ../../TokenCore/Tron/google/protobuf/generated_message_util.cc
        ../../TokenCore/Tron/google/protobuf/generated_message_util.h
        ../../TokenCore/Tron/google/protobuf/has_bits.h
        ../../TokenCore/Tron/google/protobuf/implicit_weak_message.cc
        ../../TokenCore/Tron/google/protobuf/implicit_weak_message.h
        ../../TokenCore/Tron/google/protobuf/inlined_string_field.h
        ../../TokenCore/Tron/google/protobuf/map.h
        ../../TokenCore/Tron/google/protobuf/map_entry.h
        ../../TokenCore/Tron/google/protobuf/map_entry_lite.h
        ../../TokenCore/Tron/google/protobuf/map_field.cc
        ../../TokenCore/Tron/google/protobuf/map_field.h
        ../../TokenCore/Tron/google/protobuf/map_field_inl.h
        ../../TokenCore/Tron/google/protobuf/map_field_lite.h
        ../../TokenCore/Tron/google/protobuf/map_type_handler.h
        ../../TokenCore/Tron/google/protobuf/message.cc
        ../../TokenCore/Tron/google/protobuf/message.h
        ../../TokenCore/Tron/google/protobuf/message_lite.cc
        ../../TokenCore/Tron/google/protobuf/message_lite.h
        ../../TokenCore/Tron/google/protobuf/metadata.h
        ../../TokenCore/Tron/google/protobuf/metadata_lite.h
        ../../TokenCore/Tron/google/protobuf/reflection.h
        ../../TokenCore/Tron/google/protobuf/reflection_internal.h
        ../../TokenCore/Tron/google/protobuf/reflection_ops.cc
        ../../TokenCore/Tron/google/protobuf/reflection_ops.h
        ../../TokenCore/Tron/google/protobuf/repeated_field.cc
        ../../TokenCore/Tron/google/protobuf/repeated_field.h
        ../../TokenCore/Tron/google/protobuf/service.cc
        ../../TokenCore/Tron/google/protobuf/service.h
        ../../TokenCore/Tron/google/protobuf/source_context.pb.cc
        ../../TokenCore/Tron/google/protobuf/source_context.pb.h
        ../../TokenCore/Tron/google/protobuf/struct.pb.cc
        ../../TokenCore/Tron/google/protobuf/struct.pb.h
        ../../TokenCore/Tron/google/protobuf/text_format.cc
        ../../TokenCore/Tron/google/protobuf/text_format.h
        ../../TokenCore/Tron/google/protobuf/timestamp.pb.cc
        ../../TokenCore/Tron/google/protobuf/timestamp.pb.h
        ../../TokenCore/Tron/google/protobuf/type.pb.cc
        ../../TokenCore/Tron/google/protobuf/type.pb.h
        ../../TokenCore/Tron/google/protobuf/unknown_field_set.cc
        ../../TokenCore/Tron/google/protobuf/unknown_field_set.h
        ../../TokenCore/Tron/google/protobuf/wire_format.cc
        ../../TokenCore/Tron/google/protobuf/wire_format.h
        ../../TokenCore/Tron/google/protobuf/wire_format_lite.cc
        ../../TokenCore/Tron/google/protobuf/wire_format_lite.h
        ../../TokenCore/Tron/google/protobuf/wire_format_lite_inl.h
        ../../TokenCore/Tron/google/protobuf/wrappers.pb.cc
        ../../TokenCore/Tron/google/protobuf/wrappers.pb.h
        ../../TokenCore/Tron/google/protobuf/compiler/importer.cc
        ../../TokenCore/Tron/google/protobuf/compiler/importer.h
        ../../TokenCore/Tron/google/protobuf/compiler/parser.cc
        ../../TokenCore/Tron/google/protobuf/compiler/parser.h
        ../../TokenCore/Tron/google/protobuf/io/coded_stream.cc
        ../../TokenCore/Tron/google/protobuf/io/coded_stream.h
        ../../TokenCore/Tron/google/protobuf/io/coded_stream_inl.h
        ../../TokenCore/Tron/google/protobuf/io/gzip_stream.cc
        ../../TokenCore/Tron/google/protobuf/io/gzip_stream.h
        ../../TokenCore/Tron/google/protobuf/io/printer.cc
        ../../TokenCore/Tron/google/protobuf/io/printer.h
        ../../TokenCore/Tron/google/protobuf/io/strtod.cc
        ../../TokenCore/Tron/google/protobuf/io/strtod.h
        ../../TokenCore/Tron/google/protobuf/io/tokenizer.cc
        ../../TokenCore/Tron/google/protobuf/io/tokenizer.h
        ../../TokenCore/Tron/google/protobuf/io/zero_copy_stream.cc
        ../../TokenCore/Tron/google/protobuf/io/zero_copy_stream.h
        ../../TokenCore/Tron/google/protobuf/io/zero_copy_stream_impl.cc
        ../../TokenCore/Tron/google/protobuf/io/zero_copy_stream_impl.h
        ../../TokenCore/Tron/google/protobuf/io/zero_copy_stream_impl_lite.cc
        ../../TokenCore/Tron/google/protobuf/io/zero_copy_stream_impl_lite.h
        ../../TokenCore/Tron/google/protobuf/stubs/bytestream.cc
        ../../TokenCore/Tron/google/protobuf/stubs/bytestream.h
        ../../TokenCore/Tron/google/protobuf/stubs/callback.h
        ../../TokenCore/Tron/google/protobuf/stubs/casts.h
        ../../TokenCore/Tron/google/protobuf/stubs/common.cc
        ../../TokenCore/Tron/google/protobuf/stubs/common.h
        ../../TokenCore/Tron/google/protobuf/stubs/fastmem.h
        ../../TokenCore/Tron/google/protobuf/stubs/hash.h
        ../../TokenCore/Tron/google/protobuf/stubs/int128.cc
        ../../TokenCore/Tron/google/protobuf/stubs/int128.h
        ../../TokenCore/Tron/google/protobuf/stubs/io_win32.cc
        ../../TokenCore/Tron/google/protobuf/stubs/io_win32.h
        ../../TokenCore/Tron/google/protobuf/stubs/logging.h
        ../../TokenCore/Tron/google/protobuf/stubs/macros.h
        ../../TokenCore/Tron/google/protobuf/stubs/map_util.h
        ../../TokenCore/Tron/google/protobuf/stubs/mathlimits.cc
        ../../TokenCore/Tron/google/protobuf/stubs/mathlimits.h
        ../../TokenCore/Tron/google/protobuf/stubs/mathutil.h
        ../../TokenCore/Tron/google/protobuf/stubs/mutex.h
        ../../TokenCore/Tron/google/protobuf/stubs/once.h
        ../../TokenCore/Tron/google/protobuf/stubs/platform_macros.h
        ../../TokenCore/Tron/google/protobuf/stubs/port.h
        ../../TokenCore/Tron/google/protobuf/stubs/singleton.h
        ../../TokenCore/Tron/google/protobuf/stubs/status.cc
        ../../TokenCore/Tron/google/protobuf/stubs/status.h
        ../../TokenCore/Tron/google/protobuf/stubs/statusor.cc
        ../../TokenCore/Tron/google/protobuf/stubs/statusor.h
        ../../TokenCore/Tron/google/protobuf/stubs/status_macros.h
        ../../TokenCore/Tron/google/protobuf/stubs/stl_util.h
        ../../TokenCore/Tron/google/protobuf/stubs/stringpiece.cc
        ../../TokenCore/Tron/google/protobuf/stubs/stringpiece.h
        ../../TokenCore/Tron/google/protobuf/stubs/stringprintf.cc
        ../../TokenCore/Tron/google/protobuf/stubs/stringprintf.h
        ../../TokenCore/Tron/google/protobuf/stubs/structurally_valid.cc
        ../../TokenCore/Tron/google/protobuf/stubs/strutil.cc
        ../../TokenCore/Tron/google/protobuf/stubs/strutil.h
        ../../TokenCore/Tron/google/protobuf/stubs/substitute.cc
        ../../TokenCore/Tron/google/protobuf/stubs/substitute.h
        ../../TokenCore/Tron/google/protobuf/stubs/time.cc
        ../../TokenCore/Tron/google/protobuf/stubs/time.h
        ../../TokenCore/Tron/google/protobuf/util/delimited_message_util.cc
        ../../TokenCore/Tron/google/protobuf/util/delimited_message_util.h
        ../../TokenCore/Tron/google/protobuf/util/field_comparator.cc
        ../../TokenCore/Tron/google/protobuf/util/field_comparator.h
        ../../TokenCore/Tron/google/protobuf/util/field_mask_util.cc
        ../../TokenCore/Tron/google/protobuf/util/field_mask_util.h
        ../../TokenCore/Tron/google/protobuf/util/json_util.cc
        ../../TokenCore/Tron/google/protobuf/util/json_util.h
        ../../TokenCore/Tron/google/protobuf/util/message_differencer.cc
        ../../TokenCore/Tron/google/protobuf/util/message_differencer.h
        ../../TokenCore/Tron/google/protobuf/util/time_util.cc
        ../../TokenCore/Tron/google/protobuf/util/time_util.h
        ../../TokenCore/Tron/google/protobuf/util/type_resolver.h
        ../../TokenCore/Tron/google/protobuf/util/type_resolver_util.cc
        ../../TokenCore/Tron/google/protobuf/util/type_resolver_util.h
        ../../TokenCore/Tron/google/protobuf/util/internal/constants.h
        ../../TokenCore/Tron/google/protobuf/util/internal/datapiece.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/datapiece.h
        ../../TokenCore/Tron/google/protobuf/util/internal/default_value_objectwriter.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/default_value_objectwriter.h
        ../../TokenCore/Tron/google/protobuf/util/internal/error_listener.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/error_listener.h
        ../../TokenCore/Tron/google/protobuf/util/internal/field_mask_utility.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/field_mask_utility.h
        ../../TokenCore/Tron/google/protobuf/util/internal/json_escaping.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/json_escaping.h
        ../../TokenCore/Tron/google/protobuf/util/internal/json_objectwriter.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/json_objectwriter.h
        ../../TokenCore/Tron/google/protobuf/util/internal/json_stream_parser.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/json_stream_parser.h
        ../../TokenCore/Tron/google/protobuf/util/internal/location_tracker.h
        ../../TokenCore/Tron/google/protobuf/util/internal/object_location_tracker.h
        ../../TokenCore/Tron/google/protobuf/util/internal/object_source.h
        ../../TokenCore/Tron/google/protobuf/util/internal/object_writer.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/object_writer.h
        ../../TokenCore/Tron/google/protobuf/util/internal/protostream_objectsource.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/protostream_objectsource.h
        ../../TokenCore/Tron/google/protobuf/util/internal/protostream_objectwriter.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/protostream_objectwriter.h
        ../../TokenCore/Tron/google/protobuf/util/internal/proto_writer.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/proto_writer.h
        ../../TokenCore/Tron/google/protobuf/util/internal/structured_objectwriter.h
        ../../TokenCore/Tron/google/protobuf/util/internal/type_info.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/type_info.h
        ../../TokenCore/Tron/google/protobuf/util/internal/type_info_test_helper.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/type_info_test_helper.h
        ../../TokenCore/Tron/google/protobuf/util/internal/utility.cc
        ../../TokenCore/Tron/google/protobuf/util/internal/utility.h

        #=============================================
        # TRX ?????????
        #=============================================
        ../../TokenCore/Tron/cJSON.c
        ../../TokenCore/Tron/cJSON.h
        ../../TokenCore/Tron/cJsonObject.cpp
        ../../TokenCore/Tron/cJsonObject.hpp
        ../../TokenCore/Tron/Transaction.pb.cc
        ../../TokenCore/Tron/Transaction.pb.h
#        ../../TokenCore/Tron/TronRequest.cpp
#        ../../TokenCore/Tron/TronRequest.h
        ../../TokenCore/TronApi.h
        ../../TokenCore/TronApi.cpp

        #=============================================
        # EOS
        #=============================================
#        ../../TokenCore/eosapi_internal.h
#        ../../TokenCore/eosapi_const.h
        ../../TokenCore/EosApi.h
        ../../TokenCore/EosApi.cpp

        #=============================================
        # USDT
        #=============================================
        ../../TokenCore/Usdt/createpayload.h
        ../../TokenCore/Usdt/createpayload.cpp
        #=============================================
        # BTC
        #=============================================
        ../../TokenCore/Btc/script.h
        ../../TokenCore/Btc/script.cpp
        ../../TokenCore/BtcApi.h
        ../../TokenCore/BtcApi.cpp
        #=============================================
        # ETH
        #=============================================
        ../../TokenCore/Eth/RLP.h
        ../../TokenCore/Eth/RLP.c
        ../../TokenCore/crypto/keccak256.h
        ../../TokenCore/crypto/keccak256.c
        ../../TokenCore/TokenCommon.h
        ../../TokenCore/uEcc/macroECC.h
        ../../TokenCore/uEcc/macroECC.c
        ../../TokenCore/EthApi.h
        ../../TokenCore/EthApi.cpp
        )

include_directories(
        ../../TokenCore
        ../../TokenCore/mbedtls/include
        ../../TokenCore/jsoncpp/include
#        ../../packages/boost.1.68.0.0/lib/native/include
        ../../TokenCore/Tron
)

set(TOKENCORE_SRCS
        ${TOKENCORE_COMMON_SRCS}
        src/main/cpp/nativeapi.cpp
        src/main/cpp/common.cpp
        src/main/cpp/eos.cpp
        src/main/cpp/eth.cpp
        src/main/cpp/trx.cpp
        src/main/cpp/btc.cpp
        )

add_library( tokencore
             SHARED
             ${TOKENCORE_SRCS}
             )

find_library(
                log-lib
                log
             )

target_link_libraries(  tokencore
                       ${log-lib} )