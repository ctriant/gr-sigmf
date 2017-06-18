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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "sigmf_sink_impl.h"

#include <sigmf/sigmf.h>

namespace gr {
  namespace sigmf {

    sigmf_sink::sptr
    sigmf_sink::make (const std::string& metadata_filename,
		      std::string datatype, std::string version,
		      std::string description, std::string author,
		      std::string license, std::string hw,
		      std::string sha512, double sample_rate,
		      size_t offset)
    {
      return gnuradio::get_initial_sptr (
	  new sigmf_sink_impl (metadata_filename, datatype, version,
			       description, author, license, hw,
			       sha512, sample_rate, offset));
    }

    /*
     * The private constructor
     */
    sigmf_sink_impl::sigmf_sink_impl (
	const std::string& metadata_filename, std::string datatype,
	std::string version, std::string description,
	std::string author, std::string license, std::string hw,
	std::string sha512, double sample_rate, size_t offset) :
	    gr::sync_block (
		"sigmf_sink",
		gr::io_signature::make (1, 1, sizeof(gr_complex)),
		gr::io_signature::make (0, 0, 0)),
	    d_global (datatype, version, sample_rate, sha512, offset,
		      description, author, license, hw)
    {
      d_full_w = new sigmf_writer (metadata_filename, SIGMF_FULL);
      d_full_w->append_global(d_global);
    }

    /*
     * Our virtual destructor.
     */
    sigmf_sink_impl::~sigmf_sink_impl ()
    {
      delete d_full_w;

    }

    int
    sigmf_sink_impl::work (int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      size_t nitems = std::min (noutput_items, 1024);

      std::vector<tag_t> tags;
      get_tags_in_range (tags, 0, nitems_read (0),
			 nitems_read (0) + nitems + 1);

      pmt::pmt_t val;
      if (tags.size () > 0) {
	for (size_t i = 0; i < tags.size (); i++) {
	  if (pmt::symbol_to_string (tags[i].key) == "rx_freq") {
	    capture c = capture (pmt::to_double(tags[i].value));
	    annotation a = annotation (tags[i].offset,
				       pmt::to_double(tags[i].value));

	    d_full_w->append_annotations(a);
	    d_full_w->append_captures(c);
	  }
	}
      }

      return noutput_items;
    }

  } /* namespace sigmf */
} /* namespace gr */

