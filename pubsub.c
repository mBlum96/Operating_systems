/* chat.c: Example char device module.
 *
 */
/* Kernel Programming */
#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/kernel.h>  	
#include <linux/module.h>
#include <linux/fs.h>       		
#include <asm/uaccess.h>
#include <linux/errno.h>  
#include <asm/segment.h>
#include <asm/current.h>

#include "pubsub.h"

#define MY_DEVICE "pubsub"

//#define max number of proc

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */

struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .ioctl = my_ioctl,
};,

struct myBuffer{
    char *data;
    int minor;
    int users_count;
    int write_p;
    int read_p;
}myBuffer;

myBuffer *general_buffer;
general_buffer->

int init_module(void)
{
    // This function is called when inserting the module using insmod

    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);

    if (my_major < 0)
    {
	printk(KERN_WARNING "can't get dynamic major\n");
	return my_major;
    }

    //data structure is initiate
    general_buffer = NULL;

    return 0;
}


void cleanup_module(void)
{
    // This function is called when removing the module using rmmod

    unregister_chrdev(my_major, MY_DEVICE);

    //
    // do clean_up();
    //
    return;
}


int my_open(struct inode *inode, struct file *filp)
{
    printk("my_open is called!\n");

    int


    return 0;
}


int my_release(struct inode *inode, struct file *filp)
{
    // handle file closing
    printk("my_release is called!\n");

    return 0;
}

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    //
    // Do read operation.
    // Return number of bytes read.
    return 0; 
}

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
    case SET_TYPE:
	//
	// handle 
	//
	break;
    case GET_TYPE:
	//
	// handle 
	//
	break;
    default:
	return -ENOTTY;
    }

    return 0;
}
