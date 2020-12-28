/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(time_spec.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(5bea5219931b5f044c076d80bbd415e4)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <osmosdr/time_spec.h>
// pydoc.h is automatically generated in the build directory
#include <time_spec_pydoc.h>

void bind_time_spec(py::module& m)
{

    using time_spec_t    = ::osmosdr::time_spec_t;


//    py::class_<time_spec_t, boost::operators_impl::additive<osmosdr::time_spec_t, osmosdr::time_spec_t, boost::operators_impl::operators_detail::empty_base<osmosdr::time_spec_t>, boost::operators_impl::operators_detail::false_t>,
    py::class_<time_spec_t,
        std::shared_ptr<time_spec_t>>(m, "time_spec_t", D(time_spec_t))

        .def(py::init<double>(),           py::arg("secs") = 0,
           D(time_spec_t,time_spec_t,0)
        )
        .def(py::init<time_t,double>(),           py::arg("full_secs"),
           py::arg("frac_secs") = 0,
           D(time_spec_t,time_spec_t,1)
        )
        .def(py::init<time_t,long int,double>(),           py::arg("full_secs"),
           py::arg("tick_count"),
           py::arg("tick_rate"),
           D(time_spec_t,time_spec_t,2)
        )
        .def(py::init<osmosdr::time_spec_t const &>(),           py::arg("arg0"),
           D(time_spec_t,time_spec_t,3)
        )


        
        .def_static("get_system_time",&time_spec_t::get_system_time,       
            D(time_spec_t,get_system_time)
        )


        
        .def_static("from_ticks",&time_spec_t::from_ticks,       
            py::arg("ticks"),
            py::arg("tick_rate"),
            D(time_spec_t,from_ticks)
        )


        
        .def("get_tick_count",&time_spec_t::get_tick_count,       
            py::arg("tick_rate"),
            D(time_spec_t,get_tick_count)
        )


        
        .def("to_ticks",&time_spec_t::to_ticks,       
            py::arg("tick_rate"),
            D(time_spec_t,to_ticks)
        )


        
        .def("get_real_secs",&time_spec_t::get_real_secs,       
            D(time_spec_t,get_real_secs)
        )


        
        .def("get_full_secs",&time_spec_t::get_full_secs,       
            D(time_spec_t,get_full_secs)
        )


        
        .def("get_frac_secs",&time_spec_t::get_frac_secs,       
            D(time_spec_t,get_frac_secs)
        )

        ;




}








