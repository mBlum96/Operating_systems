#!/bin/bash

make

insmod ./pubsub.o
mknod /dev/pubsub c 254 0
