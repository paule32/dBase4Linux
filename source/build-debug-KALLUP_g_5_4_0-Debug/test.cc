#include <cstddef> // NULL
#include <iomanip>
#include <iostream>
#include <ostream>
#include <istream>
#include <fstream>

#include "boost/archive/tmpdir.hpp"

#include "boost/archive/basic_archive.hpp"
#include "boost/archive/detail/basic_oserializer.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"

#include "boost/serialization/base_object.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/utility.hpp"
#include "boost/serialization/list.hpp"
#include "boost/serialization/assume_abstract.hpp"

class help_header {
public:
    ushort      file_type;
    int         version_;
    std::string password;
    int         entry_point;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & file_type;
        ar & version_;
        ar & password;
        ar & entry_point;
    }

    friend std::ostream & operator << (std::ostream &os, const help_header &hdr)
    {
        os << hdr.file_type;
        os << hdr.version_;
        os << hdr.password;
        os << hdr.entry_point;

        return os;
    }
public:
    help_header() {}

};


void check_zip(char* filename)
{
    // ----------------
    // write help() ...
    // ----------------
    std::string helpstr = R"(
    Welcome
            )";

    class help_header *hdr = new help_header;

    hdr->file_type   = 0x1949;
    hdr->version_    = 0x1501;
    hdr->entry_point = 0x0201;
    hdr->password    = std::string("passkey");

    std::ofstream out(filename);
    boost::archive::binary_oarchive oa(out);
    oa  << hdr;
    out.close();
    delete hdr;
}

int main() {
    check_zip("debug.hlp");
	return 0;
}

