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
#include <linux/slab.h>

#include "pubsub.h"

#define MY_DEVICE "pubsub"
#define MAX_CHARACTERS 1000
#define MAX_PROCESSES 512

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */
int buffers_counter = 0;

typedef struct myDevice{
    char *data;
    int minor;
    int write_p;
    int users_count;
	int readers;
    struct Process** pid_array;
    int reach_EOF_count;
	int reach_EOF_readers;
}myDevice;

typedef struct Process{
    myDevice *device;
    int pid;
    int read_p;
    int permission;
}Process;

myDevice* buffers[MAX_PROCESSES];

struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
	.write = my_write,
    .read = my_read,
    .ioctl = my_ioctl,
};

int find_available_id(Process** pid_array){
	int i;
    for (i = 0; i < MAX_PROCESSES; ++i) {
        if(pid_array[i] == NULL) return i;
    }
    //not supposed to reach here
    return 0;
}

void init_array(Process** pid_array){
	int i;
    for (i = 0; i < MAX_PROCESSES; ++i) {
        pid_array[i] = NULL;
    }
}

void init_all_read_p(Process **pid_array){
	int i;
    for (i = 0; i < MAX_PROCESSES; ++i) {
        if(pid_array[i] != NULL){
            pid_array[i]->read_p = 0;
        }
    }
}
int init_module(void)
{
    // This function is called when inserting the module using insmod

    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);

    if (my_major < 0)
    {
	printk(KERN_WARNING "can't get dynamic major\n");
	return my_major;
    }
	
	int i;
	for (i = 0; i < MAX_PROCESSES; ++i) {
        buffers[i] = NULL;
    }

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
        printk("device cannot be opened! kmalloc has failed\n");
        return -ENOMEM;
    }

    ///////////////// Matching Process With Buffer ///////////////

    int curr_minor = MINOR(inode->i_rdev);
    myDevice *curr_device;

    //checking if the buffer is already exist
   // while(curr_device){
        if(buffers[curr_minor] != NULL){
            printk("Device is found! Device's minor : %d\n", curr_minor);
			
			curr_device = buffers[curr_minor];

            new_process->device = curr_device;
			    printk("1!\n");
            new_process->pid = find_available_id(curr_device->pid_array);
						    printk("2!\n");
            new_process->read_p = 0;
            new_process->permission = TYPE_NONE;
            filp->private_data = new_process;

            curr_device->users_count++;
            curr_device->pid_array[new_process->pid] = new_process;
						    printk("3!\n");

            return 0;
        }
        //curr_device = curr_device->next;
  //  }

    //if reached here the buffer is not exist
    printk("Device was not found! Setting new device... Device's minor : %d\n", curr_minor);

    curr_device = kmalloc(sizeof(myDevice),GFP_KERNEL);
    //checking if allocating succeed
    if(!curr_device){
        printk("device cannot be opened! kmalloc has failed\n");
        return -ENOMEM;
    }
    curr_device->data = kmalloc(sizeof(char)*(MAX_CHARACTERS),GFP_KERNEL);
    //checking if allocating succeed
    if(!curr_device->data){
        printk("device cannot be opened! kmalloc has failed\n");
        kfree(curr_device);
        return -ENOMEM;
    }
    curr_device->minor = curr_minor;
    curr_device->write_p = 0;
    curr_device->users_count = 1;
	curr_device->readers = 0;

    curr_device->pid_array = kmalloc(sizeof(Process*)*(MAX_PROCESSES*2),GFP_KERNEL);
    if(!curr_device->pid_array){
            printk("device cannot be opened! kmalloc has failed\n");
            kfree(curr_device->data);
            kfree(curr_device);
            return -ENOMEM;
    }
    init_array(curr_device->pid_array);
    curr_device->pid_array[0] = new_process;

    curr_device->reach_EOF_count = 0;
	curr_device->reach_EOF_readers = 0;
	
	buffers[curr_minor] = curr_device;

    new_process->device = curr_device;
    new_process->read_p = 0;
    new_process->permission = TYPE_NONE;

    filp->private_data = new_process;

    buffers_counter++;
	printk("open succeed\n");
    return 0;
}


int my_release(struct inode *inode, struct file *filp) {
    // handle file closing
	printk("my_release is called!\n");
    Process *curr_process = (Process*)filp->private_data;
	curr_process->pid = find_available_id(curr_process->device->pid_array);
	
	printk("process id is: %d\n",curr_process->pid);

    curr_process->device->pid_array[curr_process->pid] = NULL;
    curr_process->device->users_count--;



    // last process is closed, we need to remove this buffer
    if (curr_process->device->users_count == 0) {
			printk("11111\n");
        kfree(curr_process->device->data);
        kfree(curr_process->device->pid_array);
		
		buffers[curr_process->device->minor] = NULL;
		
		kfree(curr_process->device);
		
					printk("2222222\n");

	


        buffers_counter--;
    }
	
    kfree(curr_process);

    return 0;
}

ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){

    printk("my_write is called!\n");
	

    if(buf == NULL){
        printk("Invalid input! Check your input!\n");
        return -EFAULT;
    }

    if(count > MAX_CHARACTERS){
        printk("It's more than 1000! Check your input!\n");
        return -EINVAL;
    }

    Process *curr_process = (Process*)filp->private_data;
    myDevice *curr_device = curr_process->device;
    int written_bytes = 0;

    if(curr_process->permission != TYPE_PUB){
        printk("Wrong type!\n");
        return -EACCES;
    }

    if(curr_device->write_p + count > MAX_CHARACTERS){
        printk("There is not enough space! Check your input!\n");
        return -EAGAIN;
    }
	
	
	int i = 0;
    for (; i < count; ++i) {
        //not sure if the check is necessary
        if(&buf[i] == NULL){
            printk("Buffer reading error! Check your input!\n");
            return -EBADF;
        }
        curr_device->data[curr_device->write_p] = buf[i];
        curr_device->write_p++;
        written_bytes++;
    }

    return (ssize_t)written_bytes;
}

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    //
    // Do read operation.
    printk("my_read is called!\n");

    if(buf == NULL){
        printk("Invalid buffer! Check your input!\n");
        return -EFAULT;
    }

    //there are more valid input checks?

    Process *curr_process = (Process*)filp->private_data;
    myDevice *curr_device = curr_process->device;

    // Wrong type
    if(curr_process->permission != TYPE_SUB){
        printk("Wrong type!\n");
        return -EACCES;
    }

    // No data to read
    if(curr_process->read_p == curr_device->write_p){
        printk("No data to read!\n");
        return -EAGAIN;
    }
	
	//if(curr_process->read_p + count > curr_device->write_p){
      //  printk("There is not enough data to read! Check your input!\n");
        //return -EAGAIN;
	//}

    int read_bytes = 0;
	
	int i;
    for (i = 0; i < count && curr_process->read_p < curr_device->write_p; ++i) {
        buf[i] = (curr_device->data)[curr_process->read_p];
        curr_process->read_p++;
        read_bytes++;
    }
	printk("%d\n", (int)read_bytes);

    if(curr_process->read_p == MAX_CHARACTERS){
        curr_device->reach_EOF_count++;
		curr_device->reach_EOF_readers++;
					printk("\n\n\n\nEOF\n\n\n");
					printk("EOF readers %d\n", curr_device->reach_EOF_readers);
					printk("reades %d\n", curr_device->readers);
    }

    // If all processes reached EOF, we will reset the buffer
    if(curr_device->reach_EOF_readers == curr_device->readers){
        curr_device->write_p = 0;
        init_all_read_p(curr_device->pid_array);
        curr_device->reach_EOF_count = 0;
		curr_device->reach_EOF_readers = 0;
		printk("******buffer init************");
    }
    
    // Return number of bytes read.
    return (ssize_t)read_bytes;
}

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk("my_ioctl is called!\n");
	Process* curr_process = (filp->private_data);

    switch(cmd)
    {
		case SET_TYPE:
			if(curr_process->permission != TYPE_NONE){
				printk("Error! Type is already set!\n");
				return -EPERM;
			}

			else{
				printk("Trying to set type!\n");
				if(arg == TYPE_PUB || arg == TYPE_SUB){
					if(arg == TYPE_SUB) curr_process->device->readers++;
					curr_process->permission = arg;
					printk("Type set succesfully\n");
					break;
				}
				else{
					printk("Type set was unsuccesfull\n");
					return -EINVAL;
				}
			}
			break;
		case GET_TYPE:
			if(curr_process->permission == TYPE_NONE) printk("The type is TYPE_NONE!\n");
			if(curr_process->permission == TYPE_PUB) printk("The type is TYPE_PUB!\n");
			if(curr_process->permission == TYPE_SUB) printk("The type is TYPE_SUB!\n");

			return curr_process->permission;
			break;
		default:
			return -ENOTTY;
			break;
    }

    return 0;
}

