
import fire.cfg

p = fire.cfg.Process('bench')

import sys
p.input_files = sys.argv[1:]

import os
os.makedirs('output',exist_ok=True)
p.output_file = fire.cfg.OutputFile(
        f'output/recon_{os.path.basename(p.input_files[0])}',
        rows_per_chunk = 10000,
        compression_level = 6,
        shuffle = False
        )

p.sequence = [ fire.cfg.Producer('make','bench::Recon','Bench') ]
