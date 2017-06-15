/* -*- c++ -*- */
/*
 * Copyright 2017 Kostis Triantafyllakis.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SIGMF_SIGMF_SINK_IMPL_H
#define INCLUDED_SIGMF_SIGMF_SINK_IMPL_H

#include <sigmf/sigmf_sink.h>
#include <sigmf/sigmf_writer.h>

namespace gr {
  namespace sigmf {

    class sigmf_sink_impl : public sigmf_sink
    {
      private:
	sigmf_writer *d_full_w;

	std::vector<capture> d_captures;
	std::vector<annotation> d_annotations;

	global d_global;

      public:
	sigmf_sink_impl (const std::string& metadata_filename,
			 std::string datatype, std::string version,
			 std::string description,
			 std::string author,
			 std::string license,
			 std::string hw,
			 std::string sha512,
			 double sample_rate,
			 size_t offset);
	~sigmf_sink_impl ();

	// Where all the action really happens
	int
	work (int noutput_items,
	      gr_vector_const_void_star &input_items,
	      gr_vector_void_star &output_items);

    };

  } // namespace sigmf
} // namespace gr

#endif /* INCLUDED_SIGMF_SIGMF_SINK_IMPL_H */

