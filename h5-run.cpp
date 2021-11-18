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

#include <highfive/H5File.hpp>


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

    HighFive::File file(FILE_NAME, HighFive::File::Truncate);

    HighFive::DataSpace space({1,1},{1,HighFive::DataSpace::UNLIMITED});
    HighFive::DataSetCreateProps props;
    props.add(HighFive::Chunking(std::vector<hsize_t>{1,1}));

    auto t1 = create_compound_Size2D();
    t1.commit(file, "Size2D");

    auto dataset = file.createDataSet(
        "entries", space, create_compound_Size2D(), props);

    std::vector<Size2D> dims = {{1., 2.5}, {3., 4.5}};
    for (auto const& d : dims) {
      std::vector<Size2D> entry{1,d};
      dataset.write(entry);
    }
}
