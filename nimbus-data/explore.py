#!/usr/bin/env python

import h5py
import pdb

def walk(h5object):
    #pdb.set_trace()
    if not isinstance(h5object, h5py.Dataset):
        for (k,v) in h5object.items():
            print('Subgroup %s' % v.name)
            walk(v)
    else:
        #pdb.set_trace()
        print('\tDataset %s shape %s type %s' % (h5object.name, h5object.shape, h5object.dtype))

def show(h5file):
    f = h5py.File(h5file, 'r')
    walk(f)

def main(h5file='simple.h5'):
    show(h5file)
        
if __name__ == '__main__':
    main()
