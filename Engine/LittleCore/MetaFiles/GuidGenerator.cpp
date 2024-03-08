//
// Created by Jeppe Nielsen on 08/03/2024.
//

#include "GuidGenerator.hpp"
#include <algorithm>

extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif
}

using namespace LittleCore;

std::string GuidGenerator::GenerateNew() {

#ifdef WIN32
        UUID uuid;
    UuidCreate ( &uuid );

    unsigned char * str;
    UuidToStringA ( &uuid, &str );

    std::string s( ( char* ) str );

    RpcStringFreeA ( &str );
#else
        uuid_t uuid;
        uuid_generate_random ( uuid );
        char unpacked[37];
        uuid_unparse ( uuid, unpacked );

        std::string s;
        s.reserve(32);

        for (int i = 0; i < 36; ++i) {
            if (unpacked[i] != '-') {
                s.push_back(unpacked[i]);
            }
        }

#endif

    return s;
}
