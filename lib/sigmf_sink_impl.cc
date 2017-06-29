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
    sigmf_sink::make (size_t itemsize, const std::string& metadata_filename,
		      const std::string& dataset_filename,
		      std::string datatype, std::string version,
		      std::string description, std::string author,
		      std::string license, std::string hw,
		      std::string sha512, double sample_rate,
		      size_t offset)
    {
      return gnuradio::get_initial_sptr (
	  new sigmf_sink_impl (itemsize, metadata_filename, dataset_filename,
			       datatype, version, description, author,
			       license, hw, sha512, sample_rate,
			       offset));
    }

    /*
     * The private constructor
     */
    sigmf_sink_impl::sigmf_sink_impl (
	size_t itemsize, const std::string& metadata_filename,
	const std::string& dataset_filename, std::string datatype,
	std::string version, std::string description,
	std::string author, std::string license, std::string hw,
	std::string sha512, double sample_rate, size_t offset) :
	    gr::sync_block (
		"sigmf_sink",
		gr::io_signature::make (1, 1, itemsize),
		gr::io_signature::make (0, 0, 0)),
		d_itemsize(itemsize),
	    d_full_w (
		new sigmf_writer (metadata_filename, dataset_filename,
				  SIGMF_FULL)),
	    d_global (datatype, version, sample_rate, sha512, offset,
		      description, author, license, hw),
	    file_sink_base (dataset_filename.c_str (), true, false),
	    d_msg_port (pmt::mp ("tag_msg"))
    {
      message_port_register_in (d_msg_port);
      set_msg_handler (
	  d_msg_port,
	  boost::bind (&sigmf_sink_impl::handle_tag_msg, this));

      d_msg_buf = new uint8_t[sizeof(tag_t)];

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
      char *inbuf = (char*) input_items[0];
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

      int nwritten = 0;

      do_update (); // update d_fp is reqd

      if (!d_fp)
	return noutput_items; // drop output on the floor

      while (nwritten < noutput_items) {
	int count = fwrite (inbuf, d_itemsize,
			    noutput_items - nwritten, d_fp);
	if (count == 0) {
	  if (ferror (d_fp)) {
	    std::stringstream s;
	    s << "file_sink write failed with error " << fileno (d_fp)
		<< std::endl;
	    throw std::runtime_error (s.str ());
	  }
	  else { // is EOF
	    break;
	  }
	}
	nwritten += count;
	inbuf += count * d_itemsize;
      }

      if (d_unbuffered)
	fflush (d_fp);

      return nwritten;

    }

    void
    sigmf_sink_impl::handle_tag_msg ()
    {
      uint8_t *data;
      size_t offset;
      pmt::pmt_t v = delete_head_blocking (d_msg_port);
      if (pmt::dict_has_key (v, pmt::intern ("offset"))) {
	offset = pmt::to_uint64 (
	    pmt::dict_ref (v, pmt::intern ("offset"), pmt::PMT_NIL));
      }
      else {
	offset = nitems_read (0);
      }

      /* Handle RX frequency change tag.
       * Following the format of UHD source block tags
       */
      pmt::pmt_t tag = pmt::dict_ref (v, pmt::intern ("rx_freq"),
				      pmt::PMT_NIL);
      if (!pmt::eq (tag, pmt::PMT_NIL)) {
	capture c = capture (offset);
	c.set_frequency (pmt::to_double (tag));
	d_full_w->append_captures (c);
      }

      /* Handle annotation_start tag */
      tag = pmt::dict_ref (v, pmt::intern ("annotation"),
			   pmt::PMT_NIL);
      if (!pmt::eq (tag, pmt::PMT_NIL)) {
	/* Add received tag into queue */
	if (pmt::dict_has_key (tag, pmt::intern ("sample_count"))) {
	  annotation a = annotation (
	      offset,
	      pmt::to_uint64 (
		  pmt::dict_ref (tag, pmt::intern ("sample_count"),
				 pmt::PMT_NIL)));
	  if (pmt::dict_has_key (tag,
				 pmt::intern ("freq_lower_edge"))) {
	    a.set_freq_lower_edge (
		pmt::to_double (
		    pmt::dict_ref (tag, pmt::intern ("freq_lower_edge"),
				   pmt::PMT_NIL)));
	  }
	  else if (pmt::dict_has_key (
	      tag, pmt::intern ("freq_upper_edge"))) {
	    a.set_freq_upper_edge (
		pmt::to_double (
		    pmt::dict_ref (tag, pmt::intern ("freq_upper_edge"),
				   pmt::PMT_NIL)));
	  }
	  else if (pmt::dict_has_key (tag, pmt::intern ("comment"))) {
	    a.set_comment (
		pmt::symbol_to_string (
		    pmt::dict_ref (tag, pmt::intern ("comment"),
				   pmt::PMT_NIL)));
	  }
	  else if (pmt::dict_has_key (tag,
				      pmt::intern ("generator"))) {
	    a.set_generator (
		pmt::symbol_to_string (
		    pmt::dict_ref (tag, pmt::intern ("generator"),
				   pmt::PMT_NIL)));
	  }
	}
      }

    }

    void
    sigmf_sink_impl::handle_tag (tag_t tag)
    {
      /* Handle RX frequency change tag.
       * Following the format of UHD source block tags
       */
      if (pmt::symbol_to_string (tag.key) == "rx_freq") {
	capture c = capture (tag.offset);
	c.set_frequency (pmt::to_double (tag.value));
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
		pmt::dict_ref (tmp_dict, pmt::intern ("generator"),
			       pmt::PMT_NIL));
	    a.set_generator (v);
	  }
	  if (pmt::dict_has_key (tmp_dict, pmt::intern ("comment"))) {
	    std::string v = pmt::symbol_to_string (
		pmt::dict_ref (tmp_dict, pmt::intern ("comment"),
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

