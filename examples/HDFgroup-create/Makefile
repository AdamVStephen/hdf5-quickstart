h5_create_dataset : h5_create_dataset.c
	h5cc -o $@ $<

dset.h5 : h5_create_dataset
	./h5_create_dataset

all : dset.h5

clean :
	rm -f *.o h5_create_dataset *.h5
