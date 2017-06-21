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

#include "burst_tagger_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>

namespace gr {
  namespace sigmf {

    burst_tagger::sptr
    burst_tagger::make (size_t itemsize)
    {
      return gnuradio::get_initial_sptr (
	  new burst_tagger_impl (itemsize));
    }

    burst_tagger_impl::burst_tagger_impl (size_t itemsize) :
	    sync_block (
		"burst_tagger",
		io_signature::make2 (2, 2, itemsize, sizeof(short)),
		io_signature::make (1, 1, itemsize)),
	    d_itemsize (itemsize),
	    d_freq_lower_edge (std::numeric_limits<double>::max ()),
	    d_freq_upper_edge (std::numeric_limits<double>::max ()),
	    d_state (false)
    {
      std::stringstream str;
      str << name () << unique_id ();

      d_id = pmt::string_to_symbol (str.str ());
    }

    burst_tagger_impl::~burst_tagger_impl ()
    {
    }

    void
    burst_tagger_impl::set_freq_lower_edge (double freq)
    {
      d_freq_lower_edge = freq;
    }

    void
    burst_tagger_impl::set_freq_upper_edge (double freq)
    {
      d_freq_upper_edge = freq;
    }

    void
    burst_tagger_impl::set_comment (const std::string &comment)
    {
      d_comment = comment;
    }

    void
    burst_tagger_impl::set_generator (const std::string &generator)
    {
      d_generator = generator;
    }

    void
    burst_tagger_impl::set_tag_gate (bool state)
    {
      d_state = state;
    }

    pmt::pmt_t
    burst_tagger_impl::set_tag_value ()
    {
      pmt::pmt_t value = pmt::make_dict ();
      if (d_freq_lower_edge != std::numeric_limits<double>::max ()) {
	value = pmt::dict_add (value, pmt::intern ("freq_lower_edge"),
			       pmt::from_double (d_freq_lower_edge));
      }
      if (d_freq_upper_edge != std::numeric_limits<double>::max ()) {
	value = pmt::dict_add (value, pmt::intern ("freq_upper_edge"),
			       pmt::from_double (d_freq_upper_edge));
      }
      if (!d_comment.empty ()) {
	value = pmt::dict_add (value, pmt::intern ("comment"),
			       pmt::intern (d_comment));
      }
      if (!d_generator.empty ()) {
	value = pmt::dict_add (value, pmt::intern ("generator"),
			       pmt::intern (d_generator));
      }

      return value;
    }

    int
    burst_tagger_impl::work (int noutput_items,
			     gr_vector_const_void_star &input_items,
			     gr_vector_void_star &output_items)
    {
      const char *signal = (const char*) input_items[0];
      const short *trigger = (const short*) input_items[1];
      char *out = (char*) output_items[0];

      memcpy (out, signal, noutput_items * d_itemsize);

      if (d_state) {
	add_item_tag (0, nitems_written (0),
		      pmt::string_to_symbol ("annotation_start"),
		      set_tag_value (), d_id);
	add_item_tag (0, nitems_written (0) + 1000,
		      pmt::string_to_symbol ("annotation_end"),
		      set_tag_value (), d_id);
	d_state = false;
      }
      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
