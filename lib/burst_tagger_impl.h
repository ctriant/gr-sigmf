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

#ifndef INCLUDED_SIGMF_BURST_TAGGER_IMPL_H
#define INCLUDED_SIGMF_BURST_TAGGER_IMPL_H

#include <sigmf/burst_tagger.h>

namespace gr {
  namespace sigmf {

    class burst_tagger_impl : public burst_tagger
    {
      private:
	size_t d_itemsize;

	pmt::pmt_t d_id;

	double d_freq_lower_edge;
	double d_freq_upper_edge;

	std::string d_comment;
	std::string d_generator;

	pmt::pmt_t
	set_tag_value ();

      public:
	burst_tagger_impl (size_t itemsize);
	~burst_tagger_impl ();

	void
	set_freq_lower_edge (double freq);

	void
	set_freq_upper_edge (double freq);

	void
	set_comment (const std::string &comment);

	void
	set_generator (const std::string &generator);

	int
	work (int noutput_items,
	      gr_vector_const_void_star &input_items,
	      gr_vector_void_star &output_items);
    };

  } /* namespace sigmf */
} /* namespace gr */

#endif /* INCLUDED_SIGMF_BURST_TAGGER_IMPL_H */

