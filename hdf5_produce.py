
import fire.cfg

p = fire.cfg.Process('bench')

import sys
p.event_limit = int(sys.argv[1])

import os
os.makedirs('output',exist_ok=True)
p.output_file = fire.cfg.OutputFile(f'output/output_{p.event_limit}.hdf5')

p.sequence = [ fire.cfg.Processor('make','bench::Produce','Bench') ]
