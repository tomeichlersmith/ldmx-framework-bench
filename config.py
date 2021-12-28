
from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process('bench')

import sys
p.maxEvents = int(sys.argv[1])

import os
os.makedirs('output')
p.outputFiles = ['output/output.root']

p.sequence = [ ldmxcfg.Producer('make','bench::Produce','Bench') ]
