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

#ifndef INCLUDED_SIGMF_ANNOTATION_TAGGER_H
#define INCLUDED_SIGMF_ANNOTATION_TAGGER_H

#include <sigmf/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace sigmf {

    /*!
     * \brief Sets a burst on/off tag based on the value of the trigger input.
     * \ingroup peak_detectors_blk
     * \ingroup stream_tag_tools_blk
     *
     * \details
     * This block takes two inputs, a signal stream on the input
     * stream 0 and a trigger stream of shorts on input stream 1. If
     * the trigger stream goes above 0, a tag with the key "burst"
     * will be transmitted as a pmt::PMT_T. When the trigger signal
     * falls below 0, the "burst" tag will be transmitted as
     * pmt::PMT_F.
     *
     * The signal on stream 0 is retransmitted to output stream 0.
     */
    class SIGMF_API annotation_tagger : virtual public sync_block
    {
    public:
      typedef boost::shared_ptr<annotation_tagger> sptr;

      /*!
       * Build a burst tagger gnuradio/blocks.
       *
       * \param itemsize itemsize of the signal stream on input 0.
       */
      static sptr make(size_t itemsize);


      virtual void
      set_freq_lower_edge (double freq) = 0;

      virtual void
      set_freq_upper_edge (double freq) = 0;

      virtual void
      set_comment (const std::string &comment) = 0;

      virtual void
      set_generator (const std::string &generator) = 0;

      virtual void
      set_tag_gate (bool state) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_SIGMF_ANNOTATION_TAGGER_H */

