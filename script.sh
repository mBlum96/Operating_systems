#!/bin/bash

rm -f /dev/pubsub
rmmod pubsub

make

insmod ./pubsub.o
mknod /dev/pubsub c 254 0
