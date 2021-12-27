
import ldmx.cfg

p = fire.cfg.Process('bench')

p.maxEvents = 10

p.outputFiles = ['output.root']

p.sequence = [ fire.cfg.Producer('make','bench::Produce','Bench') ]

p.pause()
