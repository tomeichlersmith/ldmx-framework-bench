/*
 *  Copyright (c), 2021, Blue Brain Project, EPFL
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 */

// Compound datatype test :: May 2021
// //////////////////////////////////

#include <highfive/H5Easy.hpp>

typedef struct {
    double width;
    double height;
} Size2D;


HighFive::CompoundType create_compound_Size2D() {
    return {{ "width",  HighFive::AtomicType<double>{} },
            { "height", HighFive::AtomicType<double>{} }};
}

HIGHFIVE_REGISTER_TYPE(Size2D, create_compound_Size2D)

int main() {
    const std::string FILE_NAME("compounds_test.h5");

    H5Easy::File file(FILE_NAME, HighFive::File::Truncate);

    auto t1 = create_compound_Size2D();
    t1.commit(file, "Size2D");

    std::vector<Size2D> dims = {{1., 2.5}, {3., 4.5}};
    for (std::size_t i_entry{0}; i_entry < dims.size(); i_entry++) {
      H5Easy::dump(file, "entries", dims.at(i_entry), {i_entry});
    }
}
