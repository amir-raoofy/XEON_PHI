#!/bin/bash

export SINK_LD_LIBRARY_PATH=/lrz/sys/intel/impi/5.1.2.150/mic/lib

export MIC_KMP_AFFINITY=explicit,granularity=fine,proclist=[1-236:1]
export KMP_AFFINITY=granularity=fine,compact,1,0
