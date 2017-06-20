/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_SIGMF_API_H
#define INCLUDED_SIGMF_API_H

#include <gnuradio/attributes.h>
#include <limits>

#ifdef gnuradio_sigmf_EXPORTS
#  define SIGMF_API __GR_ATTR_EXPORT
#else
#  define SIGMF_API __GR_ATTR_IMPORT
#endif

typedef enum sigmf_type
{
  SIGMF_FULL = 0, SIGMF_CAPTURE_ONLY, SIGMF_ANNOTATION_ONLY

} sigmfType;

#endif /* INCLUDED_SIGMF_API_H */
