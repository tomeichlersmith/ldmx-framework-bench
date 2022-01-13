
from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process('bench')

import sys
p.maxEvents = int(sys.argv[1])

import os
os.makedirs('output',exist_ok=True)
p.outputFiles = [f'output/output_{p.maxEvents}']

p.sequence = [ ldmxcfg.Producer('make','bench::Produce','Bench') ]
