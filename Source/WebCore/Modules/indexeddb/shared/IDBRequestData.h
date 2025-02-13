/*
 * Copyright (C) 2015-2020 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "IDBDatabaseIdentifier.h"
#include "IDBResourceIdentifier.h"
#include "IndexedDB.h"
#include <wtf/ArgumentCoder.h>

namespace WebCore {

class IDBOpenDBRequest;
class IDBTransaction;

namespace IndexedDB {
enum class IndexRecordType : bool;
}

namespace IDBClient {
class IDBConnectionProxy;
class TransactionOperation;
}

class IDBRequestData {
public:
    IDBRequestData(const IDBClient::IDBConnectionProxy&, const IDBOpenDBRequest&);
    explicit IDBRequestData(IDBClient::TransactionOperation&);
    WEBCORE_EXPORT IDBRequestData(const IDBRequestData&);
    IDBRequestData(IDBRequestData&&) = default;
    IDBRequestData& operator=(IDBRequestData&&) = default;

    enum IsolatedCopyTag { IsolatedCopy };
    IDBRequestData(const IDBRequestData&, IsolatedCopyTag);
    WEBCORE_EXPORT IDBRequestData isolatedCopy() const;

    IDBConnectionIdentifier serverConnectionIdentifier() const;
    WEBCORE_EXPORT IDBResourceIdentifier requestIdentifier() const;
    WEBCORE_EXPORT IDBResourceIdentifier transactionIdentifier() const;
    uint64_t objectStoreIdentifier() const;
    uint64_t indexIdentifier() const;
    IndexedDB::IndexRecordType indexRecordType() const;
    IDBResourceIdentifier cursorIdentifier() const;

    const IDBDatabaseIdentifier& databaseIdentifier() const;
    uint64_t requestedVersion() const;

    bool isOpenRequest() const { return m_requestType == IndexedDB::RequestType::Open; }
    bool isDeleteRequest() const { return m_requestType == IndexedDB::RequestType::Delete; }

    IDBRequestData isolatedCopy();

private:
    friend struct IPC::ArgumentCoder<IDBRequestData, void>;
    IDBRequestData(IDBConnectionIdentifier serverConnectionIdentifier, std::unique_ptr<IDBResourceIdentifier> requestIdentifier, std::unique_ptr<IDBResourceIdentifier> transactionIdentifier, std::unique_ptr<IDBResourceIdentifier> cursorIdentifier, uint64_t objectStoreIdentifier, uint64_t indexIdentifier, IndexedDB::IndexRecordType indexRecordType, std::optional<IDBDatabaseIdentifier> databaseIdentifier, uint64_t requestedVersion, IndexedDB::RequestType requestType)
        : m_serverConnectionIdentifier(WTFMove(serverConnectionIdentifier))
        , m_requestIdentifier(WTFMove(requestIdentifier))
        , m_transactionIdentifier(WTFMove(transactionIdentifier))
        , m_cursorIdentifier(WTFMove(cursorIdentifier))
        , m_objectStoreIdentifier(WTFMove(objectStoreIdentifier))
        , m_indexIdentifier(WTFMove(indexIdentifier))
        , m_indexRecordType(WTFMove(indexRecordType))
        , m_databaseIdentifier(WTFMove(databaseIdentifier))
        , m_requestedVersion(WTFMove(requestedVersion))
        , m_requestType(WTFMove(requestType)) { }

    static void isolatedCopy(const IDBRequestData& source, IDBRequestData& destination);

    IDBConnectionIdentifier m_serverConnectionIdentifier;
    std::unique_ptr<IDBResourceIdentifier> m_requestIdentifier;
    std::unique_ptr<IDBResourceIdentifier> m_transactionIdentifier;
    std::unique_ptr<IDBResourceIdentifier> m_cursorIdentifier;
    uint64_t m_objectStoreIdentifier { 0 };
    uint64_t m_indexIdentifier { 0 };
    IndexedDB::IndexRecordType m_indexRecordType { IndexedDB::IndexRecordType::Key };

    mutable std::optional<IDBDatabaseIdentifier> m_databaseIdentifier;
    uint64_t m_requestedVersion { 0 };

    IndexedDB::RequestType m_requestType { IndexedDB::RequestType::Other };
};

inline const IDBDatabaseIdentifier& IDBRequestData::databaseIdentifier() const
{
    ASSERT(m_databaseIdentifier);
    if (!m_databaseIdentifier)
        m_databaseIdentifier = IDBDatabaseIdentifier { };
    return *m_databaseIdentifier;
}

} // namespace WebCore
