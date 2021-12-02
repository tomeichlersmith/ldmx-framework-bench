
from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process('bench')

p.maxEvents = 10

p.outputFiles = ['output.root']

p.sequence = [ ldmxcfg.Producer('make','bench::Produce','Bench') ]

p.pause()
