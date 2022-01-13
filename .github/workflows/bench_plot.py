"""bench_plot

Plot bench park data,
this script was developed in the interactive python notebook
of the same name and then copied here for automation.
"""

import matplotlib.pyplot as plt
import pandas as pd

def bench_plot(events, hdf5_time, hdf5_size, root_time, root_size, run_mode) :
    """
    Parameters
    ----------
    events : numpy.array
        N events values benchmarked at for both hdf5 and root data
    """
    fig, ((raw_time,raw_size),(ratio_time,ratio_size)) = plt.subplots(ncols=2,nrows=2, sharex='col')
    fig.set_size_inches(11,7)
    plt.suptitle(f'Comparison Between HDF5 and ROOT : {run_mode} Mode')
    plt.subplots_adjust(wspace=0.3, hspace=0.)

    raw_time.set_ylabel('Real Time [s]')
    raw_time.set_yscale('log')
    raw_time.plot(events, hdf5_time, label='hdf5')
    raw_time.plot(events, root_time, label='root')
    raw_time.legend()

    ratio_time.set_xscale('log')
    ratio_time.set_xlabel('N Events')
    ratio_time.set_ylabel('hdf5 Time / root Time')
    ratio_time.set_ylim(bottom=0.,top=1.1)
    ratio_time.plot(events, hdf5_time/root_time, 
                label='hdf5/root', color = 'black')

    raw_size.set_ylabel('Output File Size [MB]')
    raw_size.set_yscale('log')
    raw_size.plot(events, hdf5_size/1e6)
    raw_size.plot(events, root_size/1e6)

    ratio_size.set_xscale('log')
    ratio_size.set_xlabel('N Events')
    ratio_size.set_ylabel('hdf5 Size / root Size')
    ratio_size.set_ylim(bottom=0.5,top=5.5)
    ratio_size.plot(events, hdf5_size/root_size,
               color='black')

def main() :
    import sys, os

    if len(sys.argv) < 2 :
        sys.stderr.write('ERROR: Need to provide file(s) with benchmarking data.\n')
        sys.exit(1)

    for data_file in sys.argv[1:] :
        if not os.path.isfile(data_file) :
            sys.stderr.write(f'ERROR: {data_file} not accessible.\n')
            sys.exit(2)

        data = pd.read_csv(data_file)
    
        prod = data[data['mode']=='produce']
    
        bench_plot(
            prod[prod['serializer']=='hdf5']['events'].to_numpy(),
            prod[prod['serializer']=='hdf5']['time'].to_numpy(),
            prod[prod['serializer']=='hdf5']['size'].to_numpy(),
            prod[prod['serializer']=='root']['time'].to_numpy(),
            prod[prod['serializer']=='root']['size'].to_numpy(),
            'Production')
        plt.savefig('production_'+data_file.replace('csv','pdf'))
        plt.clf()

        reco = data[data['mode']=='recon']
        bench_plot(
            reco[reco['serializer']=='hdf5']['events'].to_numpy(),
            reco[reco['serializer']=='hdf5']['time'].to_numpy(),
            reco[reco['serializer']=='hdf5']['size'].to_numpy(),
            reco[reco['serializer']=='root']['time'].to_numpy(),
            reco[reco['serializer']=='root']['size'].to_numpy(),
            'Reconstruction')
        plt.savefig('recon_'+data_file.replace('csv','pdf'))
        plt.savefig('recon_'+data_file.replace('csv','png'))
        plt.clf()

    sys.exit(0)

if __name__ == '__main__' :
    main()
