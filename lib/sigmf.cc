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
#include <sigmf/sigmf.h>
#include <iostream>

namespace gr {
  namespace sigmf {

    sigmf::sigmf (const std::string &metadata_filename,
		  sigmfType type) :
	    d_type (type)
    {
      d_doc = new rapidjson::Document ();
      set_filenames (metadata_filename);
    }

    sigmf::~sigmf ()
    {
    }

    void
    sigmf::init_object_iterators (sigmfType type)
    {
      d_sigmf_itr_begin = d_doc->MemberBegin ();
      d_sigmf_itr_end = d_doc->MemberEnd ();

      switch (type)
	{
	case SIGMF_FULL:
	  {
	    d_global_itr_begin =
		(*d_doc)["global"].GetObject ().MemberBegin ();
	    d_global_itr_end =
		(*d_doc)["global"].GetObject ().MemberEnd ();

	    d_capture_itr_begin =
		(*d_doc)["capture"].GetArray ().Begin ();
	    d_capture_itr_end =
		(*d_doc)["capture"].GetArray ().End ();

	    d_annotation_itr_begin =
		(*d_doc)["annotation"].GetArray ().Begin ();
	    d_annotation_itr_end =
		(*d_doc)["annotation"].GetArray ().End ();
	  }
	  break;
	case SIGMF_CAPTURE_ONLY:
	  {
	    d_capture_itr_begin =
		(*d_doc)["capture"].GetArray ().Begin ();
	    d_capture_itr_end =
		(*d_doc)["capture"].GetArray ().End ();
	  }
	  break;
	case SIGMF_ANNOTATION_ONLY:
	  {
	    d_annotation_itr_begin =
		(*d_doc)["annotation"].GetArray ().Begin ();
	    d_annotation_itr_end =
		(*d_doc)["annotation"].GetArray ().End ();
	  }
	  break;
	default:
	  throw std::runtime_error (
	      "init_object_iterators: invalid sigmf type");
	}
    }

    void
    sigmf::set_sigmf_itr_begin (
	const rapidjson::Value::MemberIterator& sigmfitrbegin)
    {
      d_sigmf_itr_begin = sigmfitrbegin;
    }

    void
    sigmf::set_sigmf_itr_end (
	const rapidjson::Value::MemberIterator& sigmfitrend)
    {
      d_sigmf_itr_end = sigmfitrend;
    }

    void
    sigmf::set_capture_itr_begin (
	const rapidjson::Value::ValueIterator& captureitrbegin)
    {
      d_capture_itr_begin = captureitrbegin;
    }

    void
    sigmf::set_capture_itr_end (
	const rapidjson::Value::ValueIterator& captureitrend)
    {
      d_capture_itr_end = captureitrend;
    }

    void
    sigmf::set_annotation_itr_begin (
	const rapidjson::Value::ValueIterator& annotationitrbegin)
    {
      d_annotation_itr_begin = annotationitrbegin;
    }

    void
    sigmf::set_annotation_itr_end (
	const rapidjson::Value::ValueIterator& annotationitrend)
    {
      d_annotation_itr_end = annotationitrend;
    }

    void
    sigmf::set_global_itr_begin (
	const rapidjson::Value::MemberIterator& globalitrbegin)
    {
      d_global_itr_begin = globalitrbegin;
    }

    void
    sigmf::set_global_itr_end (
	const rapidjson::Value::MemberIterator& globalitrend)
    {
      d_global_itr_end = globalitrend;
    }

    rapidjson::Value*
    sigmf::parse_global (global obj, rapidjson::Document *d)
    {
      rapidjson::Value* val = new rapidjson::Value (
	  rapidjson::kObjectType);

      if (obj.get_datatype ().empty ()) {
	throw std::runtime_error ("parse_global: datatype empty");
      }
      rapidjson::Value s (obj.get_datatype ().c_str (),
			  (*d).GetAllocator ());
      val->AddMember ("core:datatype", s, (*d).GetAllocator ());

      if (obj.get_version ().empty ()) {
	throw std::runtime_error ("parse_global: version empty");
      }
      s.SetString (obj.get_version ().c_str (), (*d).GetAllocator ());
      val->AddMember ("core:version", s, (*d).GetAllocator ());

      if (!obj.get_sha512 ().empty ()) {
	s.SetString (obj.get_sha512 ().c_str (), (*d).GetAllocator ());
	val->AddMember ("core:sha512", s, (*d).GetAllocator ());
      }

      if (!obj.get_description ().empty ()) {
	s.SetString (obj.get_description ().c_str (),
		     (*d).GetAllocator ());
	val->AddMember ("core:description", s, (*d).GetAllocator ());
      }

      if (!obj.get_author ().empty ()) {
	s.SetString (obj.get_author ().c_str (), (*d).GetAllocator ());
	val->AddMember ("core:author", s, (*d).GetAllocator ());
      }

      if (!obj.get_license ().empty ()) {
	s.SetString (obj.get_license ().c_str (), (*d).GetAllocator ());
	val->AddMember ("core:license", s, (*d).GetAllocator ());
      }

      if (!obj.get_hw ().empty ()) {
	s.SetString (obj.get_hw ().c_str (), (*d).GetAllocator ());
	val->AddMember ("core:hw", s, (*d).GetAllocator ());
      }

      if (obj.get_sample_rate () != -1) {
	val->AddMember ("core:sample_rate", obj.get_sample_rate (),
			(*d).GetAllocator ());
      }

      if (obj.get_offset () != -1) {
	val->AddMember ("core:offset", obj.get_offset (),
			(*d).GetAllocator ());
      }

      return val;
    }

    rapidjson::Value*
    sigmf::parse_capture (capture obj, rapidjson::Document *d)
    {
      rapidjson::Value* val = new rapidjson::Value (
	  rapidjson::kObjectType);

      if (obj.get_sample_start () == -1) {
	throw std::runtime_error (
	    "parse_capture: sample_start empty");
      }
      val->AddMember ("core:sample_start", obj.get_sample_start (),
		      (*d).GetAllocator ());

      if (obj.get_frequency () != -1) {
	val->AddMember ("core:frequency", obj.get_frequency (),
			(*d).GetAllocator ());
      }

      if (!obj.get_datetime ().empty ()) {
	rapidjson::Value s (obj.get_datetime ().c_str (),
			    (*d).GetAllocator ());
	val->AddMember ("core:datetime", s, (*d).GetAllocator ());
      }

      return val;
    }

    rapidjson::Value*
    sigmf::parse_annotation (annotation obj, rapidjson::Document *d)
    {
      rapidjson::Value* val = new rapidjson::Value (
	  rapidjson::kObjectType);

      if (obj.get_sample_start () == -1) {
	throw std::runtime_error (
	    "parse_annotation: sample_start empty");
      }
      val->AddMember ("core:sample_start", obj.get_sample_start (),
		      (*d).GetAllocator ());

      if (obj.get_sample_count () == -1) {
	throw std::runtime_error (
	    "parse_annotation: sample_count empty");
      }
      val->AddMember ("core:sample_count", obj.get_sample_count (),
		      (*d).GetAllocator ());

      if (obj.get_freq_lower_edge () != -1) {
	val->AddMember ("core:freq_lower_edge",
			obj.get_freq_lower_edge (),
			(*d).GetAllocator ());
      }

      if (obj.get_freq_upper_edge () != -1) {
	val->AddMember ("core:freq_upper_edge",
			obj.get_freq_upper_edge (),
			(*d).GetAllocator ());
      }

      if (!obj.get_comment ().empty ()) {
	rapidjson::Value s (obj.get_comment ().c_str (),
			    (*d).GetAllocator ());
	val->AddMember ("core:comment", s, (*d).GetAllocator ());
      }

      if (!obj.get_generator ().empty ()) {
	rapidjson::Value s (obj.get_generator ().c_str (),
			    (*d).GetAllocator ());
	val->AddMember ("core:generator", s, (*d).GetAllocator ());
      }
      return val;
    }

    sigmfType
    sigmf::get_type () const
    {
      return d_type;
    }

    void
    sigmf::set_filenames (const std::string& metadata_filename)
    {
      if (metadata_filename.empty()) {
	throw std::runtime_error (
			    "Error: Filename not given");
      }
      d_metadata_filename = metadata_filename;
      d_dataset_filename = metadata_filename;

      std::string metafile_suffix = "sigmf-meta";
      std::string dataset_suffix = "sigmf-data";
      size_t prefix_idx = metadata_filename.find_last_of (".") + 1;

      d_metadata_filename.replace (prefix_idx,
				   metafile_suffix.length (),
				   metafile_suffix);

      d_dataset_filename.replace (prefix_idx,
				  dataset_suffix.length (),
				  dataset_suffix);

      std::cout << d_metadata_filename << std::endl;
      std::cout << d_dataset_filename << std::endl;

    }

  } /* namespace sigmf */
} /* namespace gr */

