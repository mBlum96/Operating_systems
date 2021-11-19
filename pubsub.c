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
#define MAX_CHARACTERS 1000

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */
int buffer_counter = 0;

typedef struct myDevice{
    char *data;
    int minor;
    int users_count;
    int write_p;
    int read_p;
    myDevice *next;
}myDevice;

typedef struct Process{
    myDevice *device;
    int permission;
}Process;

myDevice *head_device;
myDevice *tail_device;

struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .ioctl = my_ioctl,
};

int init_module(void)
{
    // This function is called when inserting the module using insmod

    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);

    if (my_major < 0)
    {
	printk(KERN_WARNING "can't get dynamic major\n");
	return my_major;
    }

    head_device = NULL;
    tail_device = NULL;

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

    /////////////////// Allocating New Proccess ///////////////////

    //allocating new process
    Process *new_process = kmalloc(sizeof(Process),GFP_KERNEL);
    //checking ig allocating succeed
    if(!new_process){
        printk("device cannot be opened! kmalloc was failed\n");
        return -ENOMEM;
    }

    ///////////////// Matching Process With Buffer ///////////////

    int curr_minor = MINOR(inode->i_rdev);
    myDevice *curr_device = head_device;

    //checking if the buffer is already exist
    while(curr_device){
        if(curr_device->minor == curr_minor){
            printk("Device is found! Device's minor : %d\n", curr_minor);
            curr_device->users_count++;

            new_process->device = curr_device;
            new_process->permission = TYPE_NONE;
            filp->private_data = new_process;
            return 0;
        }
        curr_device = curr_device->next;
    }

    printk("Device was not found! Setting new device... Device's minor : %d\n", curr_minor);

    curr_device = kmalloc(sizeof(myDevice),GFP_KERNEL);
    //checking if allocating succeed
    if(!curr_device){
        printk("device cannot be opened! kmalloc was failed\n");
        return -ENOMEM;
    }
    curr_device->data = kmalloc(sizeof(char)*(MAX_CHARACTERS));
    //checking if allocating succeed
    if(!curr_device->data){
        printk("device cannot be opened! kmalloc was failed\n");
        kfree(curr_device);
        return -ENOMEM;
    }
    curr_device->minor = curr_minor;
    curr_device->user_count = 1;
    curr_device->write_p = 0;
    curr_device->read_p = 0;
    curr_device->next = NULL;

    new_process->device = curr_device;
    new_process->permission = TYPE_NONE;

    filp->private_data = new_process;

    if(head_device == NULL) head_device = curr_device;
    else if(tail_device == NULL) tail_device = curr_device;
    else{
        tail_device->next = curr_device;
        tail_device = curr_device;
    }
    buffer_counter++;
    return 0;
}


int my_release(struct inode *inode, struct file *filp)
{
    // handle file closing

    return 0;
}

ssize_t my_write(struct file *filp, char *buf, size_t count){

    printk("my_write is called!\n");

    if(buf == NULL){
        printk("Invalid input! Check your input!\n");
        return -EFAULT;
    }

    if(count > MAX_CHARACTERS){
        printk("It's more than 1000! Check your input!\n");
        return -EINVAL;
    }

    Process *process = filp->private_data;
    myDevice *curr_device = process->device;
    int written_bytes = 0;

    if(process->permission != TYPE_PUB){
        printk("Wrong type!\n");
        return -EACCES;
    }

    if(curr_device->write_p + count > MAX_CHARACTERS){
        printk("There is not enough space! Check your input!\n");
        return -EAGAIN;
    }

    for (int i = 0; i < count; ++i) {
        if(&buf[i] == NULL){
            printk("Buffer reading error! Check your input!\n");
            return -EBADF;
        }
        curr_device->data[curr_device->write_p] = buf[i];
        curr_device->write_p++;
        written_bytes++;
    }

    return written_bytes; //handle valid return
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
