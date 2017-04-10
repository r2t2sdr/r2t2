#!/usr/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/stefan/projekte/sdr/r2t2/svn/soft/gr-r2t2/python
export PATH=/home/stefan/projekte/sdr/r2t2/svn/soft/gr-r2t2/python:$PATH
export LD_LIBRARY_PATH=/home/stefan/projekte/sdr/r2t2/svn/soft/gr-r2t2/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/stefan/projekte/sdr/r2t2/svn/soft/gr-r2t2/swig:$PYTHONPATH
/usr/bin/python2 /home/stefan/projekte/sdr/r2t2/svn/soft/gr-r2t2/python/qa_source.py 
