/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
/*
	Improved By(Author): Madan Ram
	Email Id: madan_ram@rocketmail.com
*/
#include <cstdio>
#include <cstdlib>
#include <image.h>
#include <misc.h>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include "segment-image.h"


using namespace boost::python;
namespace np = boost::python::numeric;

/************************************************************************
	below lins of code will help in converting numpy type to ctype
	don't modify use the code as it is.

	add below line to BOOST_PYTHON_MODULE to enable numpy as type cast module
		enable_numpy_scalar_converter<boost::uint8_t, NPY_UBYTE>();
  		np::array::set_module_and_type( "numpy", "ndarray");
*************************************************************************/
/// @brief Converter type that enables automatic conversions between NumPy
///        scalars and C++ types.

#include <numpy/ndarrayobject.h>
template <typename T, NPY_TYPES NumPyScalarType>
struct enable_numpy_scalar_converter
{
  enable_numpy_scalar_converter()
  {
    // Required NumPy call in order to use the NumPy C API within another
    // extension module.
    import_array();

    boost::python::converter::registry::push_back(
      &convertible,
      &construct,
      boost::python::type_id<T>());
  }

  static void* convertible(PyObject* object)
  {
    // The object is convertible if all of the following are true:
    // - is a valid object.
    // - is a numpy array scalar.
    // - its descriptor type matches the type for this converter.
    return (
      object &&                                                    // Valid
      PyArray_CheckScalar(object) &&                               // Scalar
      PyArray_DescrFromScalar(object)->type_num == NumPyScalarType // Match
    )
      ? object // The Python object can be converted.
      : NULL;
  }

  static void construct(
    PyObject* object,
    boost::python::converter::rvalue_from_python_stage1_data* data)
  {
    // Obtain a handle to the memory block that the converter has allocated
    // for the C++ type.
    namespace python = boost::python;
    typedef python::converter::rvalue_from_python_storage<T> storage_type;
    void* storage = reinterpret_cast<storage_type*>(data)->storage.bytes;

    // Extract the array scalar type directly into the storage.
    PyArray_ScalarAsCtype(object, storage);

    // Set convertible to indicate success. 
    data->convertible = storage;
  }
};

/************************************************************************
									END
*************************************************************************/

image<rgb> *numpy_to_image(np::array& array) {
	object shape = array.attr("shape");
	int height = extract<int>(shape[0]);
    int width = extract<int>(shape[1]);

	image<rgb> *im = new image<rgb>(width, height);
	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			rgb color_pix;
			color_pix.b = extract<boost::uint8_t>(array[make_tuple(y, x, 0)])();
			color_pix.g = extract<boost::uint8_t>(array[make_tuple(y, x, 1)])();
			color_pix.r = extract<boost::uint8_t>(array[make_tuple(y, x, 2)])();
			imRef(im, x, y) = color_pix;
		}
	}
	return im;
}

list get_bounded_box(np::array& np_image, double sigma, int k, int min_size) {

  /*const char *name = name_s.c_str();*/
  int num_ccs;
  image<rgb> *im = numpy_to_image(np_image);

  std::list<bounded_box> bounded_box_list = segment_bounded_box(im, sigma, k, min_size, &num_ccs);
  list t1;
          
  int i=0;
  for(std::list<bounded_box>::iterator it=bounded_box_list.begin(); it!=bounded_box_list.end();  ++it, i++) {
    list t2;
    t2.append(it->min_x);
    t2.append(it->min_y);
    t2.append(it->max_x);
    t2.append(it->max_y);

    t1.append(t2);
    }
    return t1;
}

BOOST_PYTHON_MODULE(segment){
  enable_numpy_scalar_converter<boost::uint8_t, NPY_UBYTE>();
  np::array::set_module_and_type( "numpy", "ndarray");
  def("get_bounded_box", &get_bounded_box);
}



/*int main(int argc, char **argv) {
  if (argc != 6) {
    fprintf(stderr, "usage: %s sigma k min input(ppm) output(ppm)\n", argv[0]);
    return 1;
  }
  
  float sigma = atof(argv[1]);
  float k = atof(argv[2]);
  int min_size = atoi(argv[3]);
	
  printf("loading input image.\n");
  image<rgb> *input = loadPPM(argv[4]);
	
  printf("processing\n");
  int num_ccs; 
  image<rgb> *seg = segment_image(input, sigma, k, min_size, &num_ccs); 
  savePPM(seg, argv[5]);

  printf("got %d components\n", num_ccs);
  printf("done! uff...thats hard work.\n");

  return 0;
}*/

