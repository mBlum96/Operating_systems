#!/usr/bin/python

from __future__ import division
from struct import *
import fcntl
import struct
import os
import errno
import time

#
# Globals
#
DEVICE_PATH = '/dev/pubsub'
DEVICE_PATH2 = '/dev/pubsub2'
TYPE_NONE = 0
TYPE_PUB = 1
TYPE_SUB = 2
TYPE_HABAL=5
#
# Utilities for calculating the IOCTL command codes.
#
sizeof = {
    'byte': calcsize('c'),
    'signed byte': calcsize('b'),
    'unsigned byte': calcsize('B'),
    'short': calcsize('h'),
    'unsigned short': calcsize('H'),
    'int': calcsize('i'),
    'unsigned int': calcsize('I'),
    'long': calcsize('l'),
    'unsigned long': calcsize('L'),
    'long long': calcsize('q'),
    'unsigned long long': calcsize('Q')
}

_IOC_NRBITS = 8
_IOC_TYPEBITS = 8
_IOC_SIZEBITS = 14
_IOC_DIRBITS = 2

_IOC_NRMASK = ((1 << _IOC_NRBITS)-1)
_IOC_TYPEMASK = ((1 << _IOC_TYPEBITS)-1)
_IOC_SIZEMASK = ((1 << _IOC_SIZEBITS)-1)
_IOC_DIRMASK = ((1 << _IOC_DIRBITS)-1)

_IOC_NRSHIFT = 0
_IOC_TYPESHIFT = (_IOC_NRSHIFT+_IOC_NRBITS)
_IOC_SIZESHIFT = (_IOC_TYPESHIFT+_IOC_TYPEBITS)
_IOC_DIRSHIFT = (_IOC_SIZESHIFT+_IOC_SIZEBITS)

_IOC_NONE = 0
_IOC_WRITE = 1
_IOC_READ = 2

def _IOC(dir, _type, nr, size):
    if type(_type) == str:
        _type = ord(_type)
        
    cmd_number = (((dir)  << _IOC_DIRSHIFT) | \
        ((_type) << _IOC_TYPESHIFT) | \
        ((nr)   << _IOC_NRSHIFT) | \
        ((size) << _IOC_SIZESHIFT))

    return cmd_number

def _IO(_type, nr):
    return _IOC(_IOC_NONE, _type, nr, 0)

def _IOR(_type, nr, size):
    return _IOC(_IOC_READ, _type, nr, sizeof[size])

def _IOW(_type, nr, size):
    return _IOC(_IOC_WRITE, _type, nr, sizeof[size])


def main1(): #checking order in array after deleting something V (תלוי מימשוש שלנו , אין ASSERT פה)
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    MY_MAGIC = 'r'
    SET_TYPE = _IO(MY_MAGIC, 0)
    GET_TYPE = _IO(MY_MAGIC, 1)
    HABAL_TYPE = _IO(MY_MAGIC, 2)

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    f2 = os.open(DEVICE_PATH, os.O_RDWR)
    
    
    os.close(f)
    f3 = os.open(DEVICE_PATH, os.O_RDWR)
    
    
    
def main2():#checking my ioctl V
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    MY_MAGIC = 'r'
    SET_TYPE = _IO(MY_MAGIC, 0)
    GET_TYPE = _IO(MY_MAGIC, 1)
    HABAL_TYPE = _IO(MY_MAGIC, 2)

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    f2 = os.open(DEVICE_PATH, os.O_RDWR)
    f3 = os.open(DEVICE_PATH, os.O_RDWR)
    f4 = os.open(DEVICE_PATH, os.O_RDWR)
    g = os.open(DEVICE_PATH2, os.O_RDWR)
    
    # Check we don't have a type
    assert (fcntl.ioctl(f, GET_TYPE) == TYPE_NONE)
    assert (fcntl.ioctl(f2, GET_TYPE) == TYPE_NONE)
    assert (fcntl.ioctl(f3, GET_TYPE) == TYPE_NONE)
    
    os.close(f4)
    
    # Check we don't have a type
    assert (fcntl.ioctl(f, GET_TYPE) == TYPE_NONE)
    
    #null argument 
    #try:
    #    fcntl.ioctl(None, SET_TYPE, TYPE_SUB)   
    #    assert False 
    #except IOError, e:
    #    assert (e.errno == errno.EFAULT) 
    
    #illegal command
    try:
       fcntl.ioctl(f, HABAL_TYPE, TYPE_SUB)   
       assert False 
    except IOError, e:
       assert (e.errno == errno.ENOTTY)
        
    #illegal type 
    try:
        fcntl.ioctl(f, SET_TYPE, TYPE_HABAL)   
        assert False 
    except IOError, e:
        assert (e.errno == errno.EINVAL) 
    
    # Check we don't have a type
    assert (fcntl.ioctl(f, GET_TYPE) == TYPE_NONE)
    assert (fcntl.ioctl(f2, GET_TYPE) == TYPE_NONE)
    assert (fcntl.ioctl(f3, GET_TYPE) == TYPE_NONE)
    
    #set type success
    fcntl.ioctl(f, SET_TYPE, TYPE_PUB)
    fcntl.ioctl(f2, SET_TYPE, TYPE_SUB)
    fcntl.ioctl(f3, SET_TYPE, TYPE_SUB)
    
    # Check we have a type
    assert (fcntl.ioctl(f, GET_TYPE) == TYPE_PUB)
    assert (fcntl.ioctl(f2, GET_TYPE) == TYPE_SUB)
    assert (fcntl.ioctl(f3, GET_TYPE) == TYPE_SUB)
    
    
    # type already set
    try:
       fcntl.ioctl(f, SET_TYPE, TYPE_PUB)   
       assert False 
    except IOError, e:
       assert (e.errno == errno.EPERM)
    
    
    os.close(f)
    os.close(f2)
    os.close(f3)
    print('-------------------DONE-------------------2')

    
    
def main3(): #segel example   
        
    MY_MAGIC = 'r'
    SET_TYPE = _IO(MY_MAGIC, 0)
    GET_TYPE = _IO(MY_MAGIC, 1)
    HABAL_TYPE = _IO(MY_MAGIC, 2)

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    f2 = os.open(DEVICE_PATH, os.O_RDWR)
    f3 = os.open(DEVICE_PATH, os.O_RDWR)
    
    #set type success
    fcntl.ioctl(f, SET_TYPE, TYPE_PUB)
    fcntl.ioctl(f2, SET_TYPE, TYPE_SUB)
    fcntl.ioctl(f3, SET_TYPE, TYPE_SUB)
    
    #f------------------------------
    # We should not be able to write a very large message
    try:
        ret = os.write(f, "X" * 1001)
        assert False
    except OSError, e:
        assert (e.errno == errno.EINVAL) 
 
    ret = os.write(f, "Y" * 900)
    assert (ret == 900)
    
    # We should not be able to write ,no place
    try:
        ret = os.write(f, "X" * 101)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 
   
    #f2------------------------------
    ret= os.read(f2, 1000)
    assert (len(ret) == 900)
    
    # no more data to read
    try:
        ret = os.read(f2, 100)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 
   
    # no more data to read
    try:
        ret = os.read(f2, 300)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 

    #f3------------------------------
    ret = os.read(f3, 500)
    assert (len(ret) == 500)
    
    ret = os.read(f3, 400)
    assert (len(ret) == 400)
    
    # no more data to read
    try:
        ret = os.read(f3, 100)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 
   
    # no more data to read
    try:
        ret = os.read(f3, 300)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 
    
    #f------------------------------
    ret = os.write(f, "Z" * 101)
    assert (ret == 101)
    
    #f2------------------------------
    ret = os.read(f2, 200)
    assert (len(ret) == 101)
    
    # no more data to read
    try:
        ret = os.read(f2, 10000)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 
    
    #f3------------------------------
    ret = os.read(f3, 200)
    assert (len(ret) == 101)   
    
    # no more data to read
    try:
        ret = os.read(f3, 10000)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 
    
    # We should not be able to read
    try:
        ret = os.read(f, 100)
        assert False
    except OSError, e:
        assert (e.errno == errno.EACCES)
        
    # We should not be able to write
    try:
        ret = os.write(f2, "O" *100)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EACCES)
    
    # We should not be able to write
    try:
        ret = os.write(f3, "O" *100)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EACCES)    

    # Finaly close the device file
    os.close(f)
    os.close(f2)
    os.close(f3)
    print('-------------------DONE-------------------3')


def main4(): #checking write and read when all the data is read. with 2 different devices.
    MY_MAGIC = 'r'
    SET_TYPE = _IO(MY_MAGIC, 0)

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    f2 = os.open(DEVICE_PATH, os.O_RDWR)
    g = os.open(DEVICE_PATH2, os.O_RDWR)
    g2 = os.open(DEVICE_PATH2, os.O_RDWR)
    fcntl.ioctl(f, SET_TYPE, TYPE_PUB)
    fcntl.ioctl(f2, SET_TYPE, TYPE_SUB)
    fcntl.ioctl(g, SET_TYPE, TYPE_PUB)
    fcntl.ioctl(g2, SET_TYPE, TYPE_SUB)
    
    ret = os.write(f, "A" * 20)
    assert (ret == 20)
    ret = os.write(g, "B" * 30)
    assert (ret == 30)
    ret = os.read(f2, 10)
    ret = os.read(f2, 10)
    ret = os.read(g2, 10)
    ret = os.read(g2, 15)
    ret = os.read(g2, 5)
    ret = os.write(f, "C" * 20)
    assert (ret == 20)
    ret = os.write(g, "D" * 30)
    assert (ret == 30)
    ret = os.read(f2, 200)
    ret = os.read(g2, 200)
    
    
    os.close(f)
    os.close(f2)
    os.close(g)
    os.close(g2)     
    print('-------------------DONE-------------------4')

    
def main5(): #read return is working.
        
    MY_MAGIC = 'r'
    SET_TYPE = _IO(MY_MAGIC, 0)
    
    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    f2 = os.open(DEVICE_PATH, os.O_RDWR)
    #set type success
    fcntl.ioctl(f, SET_TYPE, TYPE_PUB)
    fcntl.ioctl(f2, SET_TYPE, TYPE_SUB)

    #f------------------------------
    ret = os.write(f, "R"*56 )
    assert (ret == 56)
   
    #f2------------------------------
    result= os.read(f2, 1000)
    assert (len(result) == 56)
    
    
    os.close(f)
    os.close(f2)
    print('-------------------DONE-------------------5')

    
    
def main(): #open->write->close->open 
        
    MY_MAGIC = 'r'
    SET_TYPE = _IO(MY_MAGIC, 0)
    GET_TYPE = _IO(MY_MAGIC, 1)
    HABAL_TYPE = _IO(MY_MAGIC, 2) 
    
    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    f2 = os.open(DEVICE_PATH, os.O_RDWR)
    #set type success
    fcntl.ioctl(f, SET_TYPE, TYPE_PUB)
    fcntl.ioctl(f2, SET_TYPE, TYPE_SUB)
    assert (fcntl.ioctl(f, GET_TYPE) == TYPE_PUB)
    assert (fcntl.ioctl(f2, GET_TYPE) == TYPE_SUB)
    #f------------------------------
    ret = os.write(f, "R"*60 )
    assert (ret == 60)
    
    #f2------------------------------
    result= os.read(f2, 20)
    assert (len(result) == 20)
    
    #f------------------------------
    os.close(f)
    #f2------------------------------
    result= os.read(f2, 10)
    assert (len(result) == 10)
    os.close(f2)
    
    f2 = os.open(DEVICE_PATH, os.O_RDWR)
    assert (fcntl.ioctl(f2, GET_TYPE) == TYPE_NONE)
    fcntl.ioctl(f2, SET_TYPE, TYPE_SUB)
    assert (fcntl.ioctl(f2, GET_TYPE) == TYPE_SUB)
    
     # no more data to read
    try:
        ret = os.read(f2, 100)
        assert False 
    except OSError, e:
        assert (e.errno == errno.EAGAIN) 

    
    f = os.open(DEVICE_PATH, os.O_RDWR)
    assert (fcntl.ioctl(f, GET_TYPE) == TYPE_NONE)
    fcntl.ioctl(f, SET_TYPE, TYPE_PUB)
    assert (fcntl.ioctl(f, GET_TYPE) == TYPE_PUB)
    
    ret = os.write(f, "E"*30 )
    assert (ret == 30)
    
    #f2------------------------------
    result= os.read(f2, 5)
    assert (len(result) == 5)
   
    os.close(f)
    os.close(f2)
    
    print('-------------------DONE-------------------6')
    
    
    
if __name__ == '__main__':
    main1()
    #main2()
    #main3()
    #main4()
    #main5()
    main()
    
    
    
