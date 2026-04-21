//
// Copyright (C) 2004-2006 Maciej Sobczak, Stephen Hutton, David Courtney
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "soci/soci.h"
#include "soci/odbc/soci-odbc.h"
#include <iostream>
#include <string>

using namespace soci;

std::string connectString;
backend_factory const &backEnd = *soci::factory_odbc();

int main(int argc, char** argv)
{
    try{
        session s(backEnd,"FILEDSN=./test-access.dsn");
        s << "create table test_binary(id integer, bin LONGBINARY)";
        binary bin{ 'H','e','l','l',' ','S','O','C','I' };
        s << "insert into test_binary([id],[bin]) values(1,?)", use(bin);

        binary result;
        s << "select bin from test_binary where id = 1",into(result);
    }
    catch (soci_error& e)
    {
        std::cerr <<e.what() <<std::endl;
        return 1;
    }
    return 0;
}
