/*
 * Copyright (c) 2002-2018 "Neo Technology,"
 * Network Engine for Objects in Lund AB [http://neotechnology.com]
 *
 * This file is part of Neo4j.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include <cstring>
#include <cstdlib>
#include <integration.hpp>
#include "catch.hpp"


struct BoltAddress * bolt_get_address(const char * host, const char * port)
{
    struct BoltAddress * address = BoltAddress_create(host, port);
    int status = BoltAddress_resolve_b(address);
    REQUIRE(status == 0);
    return address;
}

struct BoltConnection* bolt_open_b(enum BoltTransport transport, const char * host, const char * port)
{
    struct BoltAddress * address = bolt_get_address(host, port);
    struct BoltConnection* connection = BoltConnection_create();
    BoltConnection_open_b(connection, transport, address);
    BoltAddress_destroy(address);
    REQUIRE(connection->status == BOLT_CONNECTED);
    return connection;
}

struct BoltConnection* bolt_open_init_b(enum BoltTransport transport, const char * host, const char * port,
                                        const struct BoltUserProfile * profile)
{
    struct BoltConnection * connection = bolt_open_b(transport, host, port);
    BoltConnection_init_b(connection, profile);
    REQUIRE(connection->status == BOLT_READY);
    return connection;
}

void bolt_close_and_destroy_b(struct BoltConnection * connection)
{
    BoltConnection_close_b(connection);
    BoltConnection_destroy(connection);
}
