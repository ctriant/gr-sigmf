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


#ifndef INCLUDED_SIGMF_SIGMF_H
#define INCLUDED_SIGMF_SIGMF_H

#include <sigmf/api.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>

#define RAPIDJSON_BUFFER_SIZE	4096

namespace gr {
  namespace sigmf {

    /*!
     * \brief Class that represents a valid SiMF metadata file.
     *
     */
    class SIGMF_API sigmf
    {
      public:
	sigmf (const std::string &metadata_filename);

	~sigmf ();

	void
	init_object_iterators();

	void
	set_sigmf_itr_begin (
	    const rapidjson::Value::MemberIterator& sigmfitrbegin);

	void
	set_sigmf_itr_end (
	    const rapidjson::Value::MemberIterator& sigmfitrend);

	void
	set_capture_itr_begin (
	    const rapidjson::Value::ValueIterator& captureitrbegin);

	void
	set_capture_itr_end (
	    const rapidjson::Value::ValueIterator& captureitrend);

	void
	set_annotation_itr_begin (
	    const rapidjson::Value::ValueIterator& annotationitrbegin);

	void
	set_annotation_itr_end (
	    const rapidjson::Value::ValueIterator& annotationitrend);

	void
	set_global_itr_begin (
	    const rapidjson::Value::MemberIterator& globalitrbegin);

	void
	set_global_itr_end (
	    const rapidjson::Value::MemberIterator& globalitrend);

      protected:

	char d_buf[RAPIDJSON_BUFFER_SIZE];

	std::string d_metadata_filename;
	std::string d_dataset_filename;

	rapidjson::Document *d_doc;
	rapidjson::SchemaDocument *d_sd;
	rapidjson::SchemaValidator *d_validator;

	rapidjson::Value::MemberIterator d_sigmf_itr_begin;
	rapidjson::Value::MemberIterator d_sigmf_itr_end;

	rapidjson::Value::MemberIterator d_global_itr_begin;
	rapidjson::Value::MemberIterator d_global_itr_end;

	rapidjson::Value::ValueIterator d_capture_itr_begin;
	rapidjson::Value::ValueIterator d_capture_itr_end;

	rapidjson::Value::ValueIterator d_annotation_itr_begin;
	rapidjson::Value::ValueIterator d_annotation_itr_end;


      private:
	void
	set_filenames (const std::string& metadata_filename);

    };

  } // namespace sigmf
} // namespace gr

#endif /* INCLUDED_SIGMF_SIGMF_H */

