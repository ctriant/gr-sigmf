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

#include <sigmf/sigmf_reader.h>
#include <sigmf/sigmf_writer.h>
#include <sigmf/global.h>
#include <sigmf/capture.h>
#include <sigmf/annotation.h>
#include <iostream>
#include <cstdio>
#include <string>

using namespace gr::sigmf;

int
main (int argc, char** argv)
{

  std::string metadata = argv[1];
  std::string dataset = argv[2];
  sigmf_writer writer = sigmf_writer (metadata, dataset, SIGMF_FULL);

  // Create a SigMF capture segment
  capture c = capture (0);
  c.set_frequency (1e9);

  // Stream the segment to the JSON file
  writer.append_captures (c);

  // Update the SigMF capture segment
  c.set_sample_start (2000);
  c.set_frequency (2e9);

  // Append new segment to the JSON file
  writer.append_captures (c);

  // Create vector to store annotation objects
  std::vector<annotation> annotation_vec;

  // Create a SigMF annotation segment and store to vector
  annotation a = annotation (0, 100);
  a.set_freq_lower_edge (500e6);
  a.set_freq_upper_edge (1.5e9);
  a.set_comment ("Random annotation segment");
  annotation_vec.push_back (a);

  // Update the SigMF annotation segment and store to vector
  a.set_sample_start (100);
  a.set_sample_count (150);
  a.set_freq_lower_edge (500e6);
  a.set_freq_upper_edge (1.5e9);
  a.set_comment ("Another random annotation segment");
  annotation_vec.push_back (a);

  // Update the SigMF annotation segment and store to vector
  a.set_sample_start (550);
  a.set_sample_count (230);
  a.set_freq_lower_edge (700e6);
  a.set_freq_upper_edge (1.6e9);
  a.set_comment ("Final annotation segment");
  annotation_vec.push_back (a);

  // Stream annotaiton vector to the JSON file
  writer.append_annotations (annotation_vec);

  std::vector<annotation> from_json;
  sigmf_reader reader = sigmf_reader (metadata, dataset, SIGMF_FULL);
  from_json = reader.get_annotations ();

  for (size_t i = 0; i < from_json.size (); i++) {
    std::cout << "| index: " << i << " | sample_start: "
	<< from_json[i].get_sample_start () << " | sample_count: "
	<< from_json[i].get_sample_count () << " |" << std::endl;
  }

  return 0;
}
