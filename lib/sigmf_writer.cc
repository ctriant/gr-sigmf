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
#include <sigmf/sigmf_writer.h>
#include <rapidjson/pointer.h>
#include <iostream>

namespace gr {
  namespace sigmf {

    sigmf_writer::sigmf_writer (const std::string &metadata_filename,
				const std::string &dataset_filename,
				sigmfType type) :
	    sigmf (metadata_filename, dataset_filename, type)
    {
      update_writer ("w");

      init_json ();
      fclose (d_fp);

      parse ();
      init_object_iterators (d_type);
    }

    sigmf_writer::~sigmf_writer ()
    {
      delete[] d_fws;
      delete[] d_frs;
      delete[] d_writer;
    }

    void
    sigmf_writer::init_json ()
    {
      rapidjson::Value* val;
      rapidjson::Document d;

      if (d_doc->Empty ()) {
	std::cout << "Document is empty and ready to be filled"
	    << std::endl;
	d_writer->StartObject ();
	switch (d_type)
	  {
	  case SIGMF_FULL:
	    {
	      d_writer->Key ("global");
	      d_writer->StartObject ();
	      d_writer->EndObject ();

	      d_writer->Key ("capture");
	      d_writer->StartArray ();
	      d_writer->EndArray ();

	      d_writer->Key ("annotation");
	      d_writer->StartArray ();
	      d_writer->EndArray ();

	    }
	    break;

	  case SIGMF_CAPTURE_ONLY:
	    {
	      d_writer->Key ("capture");
	      d_writer->StartArray ();
	      d_writer->EndArray ();
	    }
	    break;

	  case SIGMF_ANNOTATION_ONLY:
	    {
	      d_writer->Key ("annotation");
	      d_writer->StartArray ();
	      d_writer->EndArray ();
	    }
	    break;
	  default:
	    throw std::runtime_error (
		"init_json: Invalid SigMF type");
	  }
	d_writer->EndObject ();
      }
    }

    void
    sigmf_writer::append_global (global g)
    {
      rapidjson::Document d;

      parse ();
      update_writer ("r+");

      if ((*d_doc).HasMember ("global")) {
	rapidjson::Value *v = parse_global (g, &d);
	(*d_doc)["global"].CopyFrom (*v, d.GetAllocator ());
	(*d_doc).Accept (*d_writer);
	fclose (d_fp);
      }
      else {
	throw std::runtime_error (
	    "append_global: no top-level object found");
      }
    }

    void
    sigmf_writer::append_captures (capture c)
    {
      rapidjson::Document d;

      parse ();
      update_writer ("r+");
      if ((*d_doc).HasMember ("capture")) {
	rapidjson::Value *v = parse_capture (c, &d);
	(*d_doc)["capture"].PushBack (*v, d.GetAllocator ());
	(*d_doc).Accept (*d_writer);
	fclose (d_fp);
	d_doc->GetAllocator().Clear();
      }
      else {
	throw std::runtime_error (
	    "append_captures: no top-level object found");
      }
    }

    void
    sigmf_writer::append_captures (std::vector<capture> vec)
    {
      rapidjson::Value *v;
      rapidjson::Document d;

      parse ();
      update_writer ("r+");

      if ((*d_doc).HasMember ("capture")) {
	for (size_t s = 0; s < vec.size (); s++) {
	  v = parse_capture (vec[s], &d);
	  (*d_doc)["capture"].PushBack (*v, (*d_doc).GetAllocator ());
	}
	(*d_doc).Accept (*d_writer);
	fclose (d_fp);
	d_doc->GetAllocator().Clear();
      }
      else {
	throw std::runtime_error (
	    "append_captures: no top-level object found");
      }
    }

    void
    sigmf_writer::append_annotations (annotation a)
    {
      rapidjson::Document d;

      parse ();
      update_writer ("r+");

      if ((*d_doc).HasMember ("annotation")) {
	rapidjson::Value *v = parse_annotation (a, &d);
	(*d_doc)["annotation"].PushBack (*v, d.GetAllocator ());
	(*d_doc).Accept (*d_writer);
	fclose (d_fp);
	d_doc->GetAllocator().Clear();
      }
      else {
	throw std::runtime_error (
	    "append_annotation: no top-level object found");
      }
    }

    void
    sigmf_writer::append_annotations (std::vector<annotation> vec)
    {
      rapidjson::Value *v;
      rapidjson::Document d;

      parse ();
      update_writer ("r+");

      d.SetObject ();
      if ((*d_doc).HasMember ("annotation")) {
	for (size_t s = 0; s < vec.size (); s++) {
	  v = parse_annotation (vec[s], &d);
	  (*d_doc)["annotation"].PushBack (*v, d.GetAllocator ());
	}
	(*d_doc).Accept (*d_writer);
	fclose (d_fp);
	d_doc->GetAllocator().Clear();
      }
      else {
	throw std::runtime_error (
	    "append_annotations: no top-level object found");
      }
    }

    void
    sigmf_writer::parse ()
    {
      d_fp = fopen (d_metadata_filename.c_str (), "r");
      d_frs = new rapidjson::FileReadStream (d_fp, d_buf_r,
					     sizeof(d_buf_r));
      if (d_doc->ParseStream<rapidjson::kParseIterativeFlag,
	  rapidjson::UTF8<>, rapidjson::FileReadStream> (*d_frs).HasParseError ()) {
	throw std::runtime_error (
	    GetParseError_En (d_doc->GetParseError ()));
      }
      fclose (d_fp);
    }

    void
    sigmf_writer::update_writer (std::string rights)
    {
      d_fp = fopen (d_metadata_filename.c_str (), rights.c_str ());
      d_fws = new rapidjson::FileWriteStream (d_fp, d_buf_w,
					      sizeof(d_buf_w));
      d_writer = new rapidjson::PrettyWriter<
	  rapidjson::FileWriteStream> (*d_fws);
    }

    void
    sigmf_writer::set_document (rapidjson::Document *doc)
    {
      d_doc = doc;
    }

  } /* namespace sigmf */
} /* namespace gr */

