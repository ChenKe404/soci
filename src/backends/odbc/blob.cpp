//
// Copyright (C) 2004-2006 Maciej Sobczak, Stephen Hutton, David Courtney
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define NOMINMAX
#define SOCI_ODBC_SOURCE
#include "soci/odbc/soci-odbc.h"
#include <algorithm>

using namespace soci;
using namespace soci::details;


odbc_blob_backend::odbc_blob_backend(odbc_session_backend &session)
    : session_(session),stmt_(nullptr)
{
}

odbc_blob_backend::~odbc_blob_backend()
{
    // ...
}

std::size_t odbc_blob_backend::get_len()
{
    return buffer_.size();
}

std::size_t odbc_blob_backend::read(
    std::size_t offset, char * buf, std::size_t toRead)
{
    const auto total = buffer_.size();
    if(offset >= total || toRead < 1)
        return 0;
    auto size = std::min(total - offset,toRead);
    memcpy(buf,buffer_.data() + offset, size);
    return size;
}

std::size_t odbc_blob_backend::write(
    std::size_t offset, char const * buf,
    std::size_t toWrite)
{
    if(toWrite < 1)
        return 0;
    const auto total = std::max(buffer_.size(),offset + toWrite);
    buffer_.resize(total);
    memcpy(buffer_.data() + offset, buf,toWrite);
    return toWrite;
}

std::size_t odbc_blob_backend::append(
    char const * buf, std::size_t toWrite)
{
    if(toWrite < 1)
        return 0;
    buffer_.resize(buffer_.size() + toWrite);
    memcpy(buffer_.data() + buffer_.size(), buf, toWrite);
    return toWrite;
}

void odbc_blob_backend::trim(std::size_t newLen)
{
    buffer_.resize(newLen);
}

void odbc_blob_backend::bind(odbc_statement_backend* stmt,int column, mode mode)
{
    if(!stmt_ || &stmt->session_ != &session_)
        return;
    stmt_ = stmt;
    mode_ = mode;
    column_ = column;
    if(mode != Read)
        return;

    auto hstmt = stmt_->hstmt_;
    SQLLEN len = 0;
    auto pos = static_cast<SQLSMALLINT>(column_);
    char c;
    // get length
    SQLRETURN rc = SQLGetData(hstmt, pos, SQL_C_BINARY, &c, 0, &len);
    if (is_odbc_error(rc))
        throw odbc_soci_error(SQL_HANDLE_STMT, hstmt, "get blob length");
    if(len < 1)
        return;
    // read blob data to buffer
    buffer_.resize(len);
    rc = SQLGetData(hstmt, pos, SQL_C_BINARY, buffer_.data(), buffer_.size(), &len);
    if (is_odbc_error(rc))
        throw odbc_soci_error(SQL_HANDLE_STMT, hstmt, "read blob data");
}
