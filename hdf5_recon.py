
import fire.cfg

p = fire.cfg.Process('bench')

import sys
p.input_files = sys.argv[1:]

import os
os.makedirs('output',exist_ok=True)
p.output_file = fire.cfg.OutputFile(f'output/recon_{os.path.basename(p.input_files[0]).replace(".root","_root.hdf5")}')

# keep everything for better comparison to old framework
p.keep('.*')

p.sequence = [ fire.cfg.Processor('make','bench::Recon','Bench') ]
