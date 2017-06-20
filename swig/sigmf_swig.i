/* -*- c++ -*- */

#define SIGMF_API

%include "gnuradio.i"

//load generated python docstrings
%include "sigmf_swig_doc.i"

%{

#include "sigmf/sigmf_sink.h"
#include "sigmf/burst_tagger.h"
%}

%include "sigmf/sigmf_sink.h"

GR_SWIG_BLOCK_MAGIC2(sigmf, sigmf_sink);
%include "sigmf/burst_tagger.h"
GR_SWIG_BLOCK_MAGIC2(sigmf, burst_tagger);
