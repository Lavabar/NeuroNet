# NeuroNet
Network has 2 functions:
one for detecting guns and not guns(works with webcamera)
other for using image-base CIFAR-10


# Files
### Makefile
makefile
### prefix 'ipl...'
files with prefix 'ipl' manage work with v4l-driver

### net_structs
contains structure of neuronet
### netcreat
functions for creating new neuronets
### netfile
functions for reading and writing data in files
### netpass 
in this file are functions of forward pass and backward pass
### train.c
main-file for teaching neuronet

### neurowork
function for processing image by neuronet(only detecting guns and notguns)

### test.c
it is main-file for working with image-base CIFAR-10
### main.c
main-file for working with gun detector

