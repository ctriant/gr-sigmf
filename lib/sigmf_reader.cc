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
#include <sigmf/sigmf_reader.h>
#include <iostream>

namespace gr {
  namespace sigmf {

    sigmf_reader::sigmf_reader (const std::string &metadata_filename,
				const std::string &dataset_filename,
				sigmfType type) :
	    sigmf (metadata_filename, dataset_filename, type)
    {
      d_fp = fopen (metadata_filename.c_str (), "r");
      d_frs = new rapidjson::FileReadStream (d_fp, d_buf_r,
					     sizeof(d_buf_r));
      if (d_doc->ParseStream<rapidjson::kParseStopWhenDoneFlag, rapidjson::UTF8<>, rapidjson::FileReadStream> (
	  *d_frs).HasParseError ()) {
	throw std::runtime_error (
	    GetParseError_En (d_doc->GetParseError ()));
      }
      fclose (d_fp);
      init_object_iterators (d_type);
    }

    sigmf_reader::~sigmf_reader ()
    {

    }

    global
    sigmf_reader::get_global ()
    {
      if (!(*d_doc).HasMember("global")) {
	throw std::runtime_error (
		    "get_global: no global object found");
      }
      if (!d_type == SIGMF_FULL) {
	throw std::runtime_error (
	    "get_global: parsing wrong SigMF file type");
      }

      global g;
      for (rapidjson::Value::MemberIterator itr = d_global_itr_begin;
	  itr != d_global_itr_end; ++itr) {
	switch (itr->value.GetType ())
	  {
	  case rapidjson::kStringType:
	    {
	      std::string key = itr->name.GetString ();
	      std::string value = itr->value.GetString ();
	      if (key == "core:datatype") {
		g.set_datatype (value);
	      }
	      else if (key == "core:version") {
		g.set_version (value);
	      }
	      else if (key == "core:sha512") {
		g.set_sha512 (value);
	      }
	      else if (key == "core:description") {
		g.set_description (value);
	      }
	      else if (key == "core:author") {
		g.set_author (value);
	      }
	      else if (key == "core:license") {
		g.set_license (value);
	      }
	      else if (key == "core:hw") {
		g.set_hw (value);
	      }
	      else {
		std::string error = "global: Invalid string field "
		    + key;
		throw std::runtime_error (error);
	      }
	    }
	    break;
	  case rapidjson::kNumberType:
	    {
	      std::string key = itr->name.GetString ();
	      if (key == "core:sample_rate") {
		double value = itr->value.GetDouble ();
		g.set_sample_rate (value);
	      }
	      else if (key == "core:offset") {
		size_t value = itr->value.GetInt64 ();
		g.set_offset (value);
	      }
	      else {
		throw std::runtime_error (
		    "global: Invalid numeric field");
	      }
	    }
	    break;
	  default:
	    {
	      throw std::runtime_error (
		  "global: Invalid field datatype");
	    }
	    break;
	  }
      }
      return g;
    }

    capture
    sigmf_reader::get_capture (rapidjson::Value::MemberIterator begin,
			       rapidjson::Value::MemberIterator end)
    {
      if (!(*d_doc).HasMember("capture")) {
	throw std::runtime_error (
		    "get_capture: no capture object found");
      }
      if (d_type == SIGMF_ANNOTATION_ONLY) {
	throw std::runtime_error (
	    "get_capture: parsing SIGMF_ANNOTATION_ONLY");
      }

      capture c;
      for (rapidjson::Value::MemberIterator itr = begin; itr != end;
	  ++itr) {
	switch (itr->value.GetType ())
	  {
	  case rapidjson::kStringType:
	    {
	      std::string key = itr->name.GetString ();
	      std::string value = itr->value.GetString ();
	      if (key == "core:datetime") {
		c.set_datetime (value);
	      }
	      else {
		std::string error = "capture: Invalid string field "
		    + key;
		throw std::runtime_error (error);
	      }
	    }
	    break;
	  case rapidjson::kNumberType:
	    {
	      std::string key = itr->name.GetString ();
	      if (key == "core:frequency") {
		double value = itr->value.GetDouble ();
		c.set_frequency (value);
	      }
	      else if (key == "core:sample_start") {
		size_t value = itr->value.GetInt64 ();
		c.set_sample_start (value);
	      }
	      else {
		throw std::runtime_error (
		    "capture: Invalid numeric field");
	      }
	    }
	    break;
	  default:
	    {
	      throw std::runtime_error (
		  "capture: Invalid field datatype");
	    }
	    break;
	  }
      }
      return c;
    }

    std::vector<capture>
    sigmf_reader::get_captures ()
    {
      if (!(*d_doc).HasMember("capture")) {
	throw std::runtime_error (
		    "get_capture: no capture object found");
      }
      if (d_type == SIGMF_ANNOTATION_ONLY) {
	throw std::runtime_error (
	    "get_capture: parsing SIGMF_ANNOTATION_ONLY");
      }
      std::vector<capture> captures;
      capture c;
      for (rapidjson::Value::ValueIterator itr = d_capture_itr_begin;
	  itr != d_capture_itr_end; ++itr) {
	c = get_capture (itr->MemberBegin (), itr->MemberEnd ());
	captures.push_back (c);
      }
      return captures;
    }

    annotation
    sigmf_reader::get_annotation (
	rapidjson::Value::MemberIterator begin,
	rapidjson::Value::MemberIterator end)
    {
      if (!(*d_doc).HasMember("annotation")) {
	throw std::runtime_error (
		    "get_annotation: no annotation object found");
      }
      if (d_type == SIGMF_CAPTURE_ONLY) {
	throw std::runtime_error (
	    "get_annotation: parsing SIGMF_CAPTURE_ONLY");
      }
      annotation a;
      for (rapidjson::Value::MemberIterator itr = begin; itr != end;
	  ++itr) {
	switch (itr->value.GetType ())
	  {
	  case rapidjson::kStringType:
	    {
	      std::string key = itr->name.GetString ();
	      std::string value = itr->value.GetString ();
	      if (key == "core:generator") {
		a.set_generator (value);
	      }
	      else if (key == "core:comment") {
		a.set_comment (value);
	      }
	      else {
		std::string error =
		    "annotation: Invalid string field " + key;
		throw std::runtime_error (error);
	      }
	    }
	    break;
	  case rapidjson::kNumberType:
	    {
	      std::string key = itr->name.GetString ();
	      if (key == "core:freq_lower_edge") {
		double value = itr->value.GetDouble ();
		a.set_freq_lower_edge (value);
	      }
	      else if (key == "core:freq_upper_edge") {
		size_t value = itr->value.GetDouble ();
		a.set_freq_upper_edge (value);
	      }
	      else if (key == "core:sample_start") {
		size_t value = itr->value.GetInt64 ();
		a.set_sample_start (value);
	      }
	      else if (key == "core:sample_count") {
		size_t value = itr->value.GetInt64 ();
		a.set_sample_count (value);
	      }
	      else {
		throw std::runtime_error (
		    "annotation: Invalid numeric field");
	      }
	    }
	    break;
	  default:
	    {
	      throw std::runtime_error (
		  "annotation: Invalid field datatype");
	    }
	    break;
	  }
      }
      return a;
    }

    std::vector<annotation>
    sigmf_reader::get_annotations ()
    {
      if (!(*d_doc).HasMember("annotation")) {
	throw std::runtime_error (
		    "get_annotation: no annotation object found");
      }
      if (d_type == SIGMF_CAPTURE_ONLY) {
	throw std::runtime_error (
	    "get_annotation: parsing SIGMF_CAPTURE_ONLY");
      }
      std::vector<annotation> annotations;
      annotation a;
      for (rapidjson::Value::ValueIterator itr =
	  d_annotation_itr_begin; itr != d_annotation_itr_end;
	  ++itr) {
	a = get_annotation (itr->MemberBegin (), itr->MemberEnd ());
	annotations.push_back (a);
      }
      return annotations;
    }

    rapidjson::Document*
    sigmf_reader::get_document () {
      return d_doc;
    }

  } /* namespace sigmf */
} /* namespace gr */

