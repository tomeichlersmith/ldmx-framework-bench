
import fire.cfg

p = fire.cfg.Process('bench')

import sys
p.event_limit = int(sys.argv[1])

import os
os.makedirs('output')
p.output_file = fire.cfg.OutputFile(f'output/output_bench_{p.event_limit}.h5')

p.sequence = [ fire.cfg.Producer('make','bench::Produce','Bench') ]
