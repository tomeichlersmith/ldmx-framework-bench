
import fire.cfg

p = fire.cfg.Process('bench')

import sys
p.event_limit = int(sys.argv[1])

import os
os.makedirs('output',exist_ok=True)
p.output_file = fire.cfg.OutputFile(
        f'output/output_{p.event_limit}.h5',
        rows_per_chunk = 10000,
        compression_level = 6,
        shuffle = False
        )

p.sequence = [ fire.cfg.Producer('make','bench::Produce','Bench') ]
