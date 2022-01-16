
from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process('bench')

import sys
p.inputFiles = sys.argv[1:]

import os
os.makedirs('output',exist_ok=True)
p.outputFiles = [f'output/recon_{os.path.basename(p.inputFiles[0])}']
p.sequence = [ ldmxcfg.Producer('make','bench::Recon','Bench') ]
