explore : explore.c
	h5cc -g -o $@ $<

# Not required once the correct 1.12.0 versions are being used.
# h5cc -DH5_USE16_API -o $@ $<
#	h5cc -DH5_USE16_API -o $@ $<

test : explore simple.h5
	./explore simple.h5

all : explore

clean :
	rm -f *.o explore 
