"""bench_plot

Plot bench park data,
this script was developed in the interactive python notebook
of the same name and then copied here for automation.
"""

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def bench_plot(data, run_mode, selections) :
    """
    Parameters
    ----------
    events : numpy.array
        N events values benchmarked at for both hdf5 and root data
    """
    fig, ((raw_time,raw_size),(ratio_time,ratio_size)) = plt.subplots(ncols=2,nrows=2, 
        sharex='col', gridspec_kw = { 'height_ratios' : [ 3, 1] })
    fig.set_size_inches(11,7)
    plt.suptitle(f'Comparison Between HDF5 and ROOT : {run_mode} Mode')
    plt.subplots_adjust(wspace=0.3, hspace=0.)

    events = data['events'].unique()

    raw_time.set_ylabel('Real Time [s]')
    raw_time.set_yscale('log')
    for name, sl in selections.items() :
        raw_time.plot(events, data[sl]['time'], label=name)
    raw_time.legend()

    ratio_time.set_xscale('log')
    ratio_time.set_xlabel('N Events')
    first_key = None
    first_sl = None
    for name, sl in selections.items() :
        if first_key is None :
            first_key = name
            first_sl = sl
            ratio_time.plot(events, np.ones(len(events)))
        else :
            ratio_time.plot(events, data[sl]['time'].to_numpy()/data[first_sl]['time'].to_numpy(),
                            label=name)
    ratio_time.set_ylabel('Ratio to {first_key}')

    raw_size.set_ylabel('Output File Size [MB]')
    raw_size.set_yscale('log')
    for name, sl in selections.items() :
        raw_size.plot(events, data[sl]['size']/1e6, label=name)
    raw_size.legend()

    ratio_size.set_xscale('log')
    ratio_size.set_xlabel('N Events')
    first_key = None
    first_sl = None
    for name, sl in selections.items() :
        if first_key is None :
            first_key = name
            first_sl = sl
            ratio_size.plot(events, np.ones(len(events)))
        else :
            ratio_size.plot(events, data[sl]['size'].to_numpy()/data[first_sl]['size'].to_numpy(), 
                            label=name)
    ratio_size.set_ylabel('Ratio to {first_key}')

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
    
        bench_plot(data,'Production',
            {
              'root' : (data['mode']=='produce')&(data['writer']=='root')&(data['reader']=='root'),
              'hdf5' : (data['mode']=='produce')&(data['writer']=='hdf5')&(data['reader']=='hdf5')
            })
        plt.savefig('production_'+data_file.replace('csv','pdf'))
        plt.savefig('production_'+data_file.replace('csv','png'))
        plt.clf()

        bench_plot(data, 'Reconstruction',
            {
              'root reco root' : (data['mode']=='recon')&(data['writer']=='root')&(data['reader']=='root'),
              'hdf5 reco root' : (data['mode']=='recon')&(data['writer']=='root')&(data['reader']=='hdf5'),
              'hdf5 reco hdf5' : (data['mode']=='recon')&(data['writer']=='hdf5')&(data['reader']=='hdf5')
            })
        plt.savefig('recon_'+data_file.replace('csv','pdf'))
        plt.savefig('recon_'+data_file.replace('csv','png'))
        plt.clf()

    sys.exit(0)

if __name__ == '__main__' :
    main()
