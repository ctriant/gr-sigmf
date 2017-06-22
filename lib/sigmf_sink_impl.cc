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
      d_full_w->append_global (d_global);
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
	  handle_tag (tags[i]);
	}
      }

      return noutput_items;
    }

    void
    sigmf_sink_impl::handle_tag (tag_t tag)
    {
      /* Handle RX frequency change tag.
       * Following the format of UHD source block tags
       */
      if (pmt::symbol_to_string (tag.key) == "rx_freq") {
	capture c = capture (tag.offset);
	c.set_frequency(pmt::to_double (tag.value));
	d_full_w->append_captures (c);
      }
      /* Handle annotation_start tag */
      if (pmt::symbol_to_string (tag.key) == "annotation_start") {
	/* Add received tag into queue */
	d_annot_tag_queue.push (tag);
      }
      if (pmt::symbol_to_string (tag.key) == "annotation_end") {
	pmt::pmt_t tmp_dict;
	d_last_tag_rcvd = d_annot_tag_queue.front ();
	d_annot_tag_queue.pop ();
	tmp_dict = d_last_tag_rcvd.value;
	annotation a = annotation (
	    d_last_tag_rcvd.offset,
	    tag.offset - d_last_tag_rcvd.offset);
	if (!pmt::eq (tmp_dict, pmt::PMT_NIL)) {
	  if (pmt::dict_has_key (tmp_dict,
				 pmt::intern ("freq_lower_edge"))) {
	    double v = pmt::to_double (
		pmt::dict_ref (tmp_dict,
			       pmt::intern ("freq_lower_edge"),
			       pmt::PMT_NIL));
	    a.set_freq_lower_edge (v);
	  }
	  if (pmt::dict_has_key (tmp_dict,
				 pmt::intern ("freq_upper_edge"))) {
	    double v = pmt::to_double (
		pmt::dict_ref (tmp_dict,
			       pmt::intern ("freq_upper_edge"),
			       pmt::PMT_NIL));
	    a.set_freq_upper_edge (v);
	  }
	  if (pmt::dict_has_key (tmp_dict,
				 pmt::intern ("generator"))) {
	    std::string v = pmt::symbol_to_string (
		pmt::dict_ref (tmp_dict,
			       pmt::intern ("generator"),
			       pmt::PMT_NIL));
	    a.set_generator (v);
	  }
	  if (pmt::dict_has_key (tmp_dict,
				 pmt::intern ("comment"))) {
	    std::string v = pmt::symbol_to_string (
		pmt::dict_ref (tmp_dict,
			       pmt::intern ("comment"),
			       pmt::PMT_NIL));
	    a.set_comment (v);
	  }
	}
	// TODO: Parse tag values to extract other annotation fields
	d_full_w->append_annotations (a);
      }
    }

  } /* namespace sigmf */
} /* namespace gr */

