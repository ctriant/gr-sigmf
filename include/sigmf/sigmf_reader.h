/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SIGMF_SIGMF_READER_H
#define INCLUDED_SIGMF_SIGMF_READER_H

#include <sigmf/api.h>
#include <vector>
#include <sigmf/sigmf.h>
#include <rapidjson/filereadstream.h>

namespace gr {
  namespace sigmf {

    /*!
     * \brief <+description+>
     *
     */
    class SIGMF_API sigmf_reader : public sigmf
    {
      public:
	sigmf_reader (const std::string &metadata_filename,
		      const std::string &dataset_filename,
		      sigmfType type);
	~sigmf_reader ();

	global
	get_global ();

	capture
	get_capture (rapidjson::Value::MemberIterator begin,
		     rapidjson::Value::MemberIterator end);

	std::vector<capture>
	get_captures ();

	annotation
	get_annotation (rapidjson::Value::MemberIterator begin,
			rapidjson::Value::MemberIterator end);

	std::vector<annotation>
	get_annotations ();

	rapidjson::Document*
	get_document ();

      private:
	FILE* d_fp;
	rapidjson::FileReadStream *d_frs;
    };

  } // namespace sigmf
} // namespace gr

#endif /* INCLUDED_SIGMF_SIGMF_READER_H */

