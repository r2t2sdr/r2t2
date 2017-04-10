/* -*- c++ -*- */

#define R2T2_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "r2t2_swig_doc.i"

%{
#include "r2t2/sink.h"
#include "r2t2/source.h"
%}


%include "r2t2/sink.h"
GR_SWIG_BLOCK_MAGIC2(r2t2, sink);
%include "r2t2/source.h"
GR_SWIG_BLOCK_MAGIC2(r2t2, source);
