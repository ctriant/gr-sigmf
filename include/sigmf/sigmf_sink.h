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

#ifndef INCLUDED_SIGMF_SIGMF_SINK_H
#define INCLUDED_SIGMF_SIGMF_SINK_H

#include <sigmf/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/blocks/file_sink_base.h>

namespace gr {
  namespace sigmf {

    /*!
     * \brief <+description of block+>
     * \ingroup sigmf
     *
     */
    class SIGMF_API sigmf_sink : virtual public gr::sync_block,
	virtual public gr::blocks::file_sink_base

    {
      public:
	typedef boost::shared_ptr<sigmf_sink> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of sigmf::sigmf_sink.
	 *
	 * To avoid accidental use of raw pointers, sigmf::sigmf_sink's
	 * constructor is in a private implementation
	 * class. sigmf::sigmf_sink::make is the public interface for
	 * creating new instances.
	 */
	static sptr
	make (size_t itemsize, const std::string& metadata_filename,
	      const std::string& dataset_filename,
	      std::string datatype, std::string version,
	      std::string description = "", std::string author = "",
	      std::string license = "", std::string hw = "",
	      std::string sha512 = "", double sample_rate = -1.0,
	      size_t offset = 0);
    };

  } // namespace sigmf
} // namespace gr

#endif /* INCLUDED_SIGMF_SIGMF_SINK_H */

