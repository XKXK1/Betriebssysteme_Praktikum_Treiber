#include "chardev.h"

struct mutex myMutex;
static int chardev_major; //Major Number which is the Device Number
static int anzZeichen; 
static u64 zeit_stempel;
static int passedTime;
static struct cdev my_cdev; // the kernel uses structures of type struct cdev to represent char devices internally
static int open = 0;
static int writeTrue = 0;
static int ret_val_time = DEF_RET_VAL_TIME ;
static int ret_val_number = DEF_RET_VAL_NUMBER;

// Following File Operations(which the device can perfom on his Nodes) will be defined:


/*
If a char device is opened for the first time "open" will be set to 1. 
If not, -EBUSY will be returned.
*/
int chardev_open(struct inode *inode, struct file *filp) {
	if(open ==0){
	PDEBUG("Chardev: OPEN\n");
	open =1;
	}else{
	  return -EBUSY;
	}
	return 0;
}

/*
If the char device freed open will be set to 0, 
which allows a new device to be made.
*/
int chardev_release(struct inode *inode, struct file *filp) {
	PDEBUG("Chardev: RELEASE\n");
	open = 0;
	return 0;
}

/*
Data will be read from the device.
*/
ssize_t chardev_read(struct file *filp
, char *buffer,//buffer which will be filled with the message
size_t buffer_length,//length of the message
loff_t *f_offset//file offset
){
	int ubergeben = 0;
	char ausgabe_buffer[BUFFER_SIZE] = { 0 };
	
	
	if(*f_offset != 0){ //second read call
		return 0;
	}
	// if something got already written return the real values of length and time since last message
	if (writeTrue == 1){
	  sprintf(ausgabe_buffer,"Char Count: %i  Passed Time: %u\n",anzZeichen,passedTime);
	// if something is written for the first time return predefined values for length and time since last message
	} else {
	  sprintf(ausgabe_buffer,"Char Count: %i  Passed Time: %i\n",ret_val_number,ret_val_time);
	  }
	
	
	ubergeben = MINIMUM(strlen(ausgabe_buffer),buffer_length);
	
	mutex_lock(&myMutex); // locking mutex to prevent "write" from accessing at the same time
	
	//copy to user checks whether the memory is accessible
	if(copy_to_user(buffer,ausgabe_buffer,ubergeben)){
		PDEBUG("Chardev: No data end");
	}
	else{
		PDEBUG("Chardev: data send: OK");
	}
	
	PDEBUG("Chardev: Input length %d read\n", ubergeben);
	
	// returning possible offset
	*f_offset = ubergeben;
	mutex_unlock(&myMutex);// unlock the mutex
	return ubergeben;
	
}

/*
Data will be written to the device.
*/
ssize_t chardev_write(struct file *filp,const char *buffer,size_t buffer_length,loff_t *f_offset){
	
	u64 temp = 0; // temporary time measuring variable
	mutex_lock(&myMutex);// locking mutex to prevent "read" from accessing at the same time
	anzZeichen = buffer_length-1; // length of the message
	PDEBUG("Chardev: Input length %d char %s read: OK\n", buffer_length,buffer);
	
	temp = (u64)get_jiffies_64(); //getting the first timestamp
	
	//if its the first time the timestamp has to be taken here
	if(writeTrue==0){
	zeit_stempel = temp;
	}
	
	//calculating time since last message
	passedTime = temp - zeit_stempel;
	passedTime = passedTime * 1000 / HZ;
	zeit_stempel = temp;
	
	PDEBUG("Chardev: Write: OK\n");
	mutex_unlock(&myMutex);
	writeTrue = 1; // now a message has already been written
	return buffer_length;
}

/*
Linking the file operations which are going to be implemented
*/

struct file_operations fops = {
       .read = chardev_read,
       .write = chardev_write,
       .open = chardev_open,
       .release = chardev_release
    };

	
/*
Initialising the char_device


*/
static int chardev_init(void){
	int result; // possible error variable
	dev_t dev = 0; // device type which saves the device Number(Major,Minor)
	
	mutex_init(&myMutex); 
	
	PDEBUG("Chardev: Initialisation started\n");
	
	//dynamic allocation of major numbers to easily pick a number which is not already used
	result = alloc_chrdev_region(&dev,MINOR_START,DEVICE_COUNT,"chardev"); 
	if (result < 0) {
		printk(KERN_WARNING "Cannot register Major number for timer!");
		return result;
	}
	
	
	chardev_major = MAJOR(dev); // assigning the major number to the variable
	
	// Variables which are used for the funcionality of the Module
	anzZeichen = 0;
	passedTime = 0;
	
	//registration of a char device which is linked to the implemented file operations
	cdev_init(&my_cdev, &fops); 
	
	// finally adding the char_device to the kernel with the created device number.
	result = cdev_add(&my_cdev, MKDEV(chardev_major, MINOR_START), 1);
	  if (result < 0) {
	    printk(KERN_WARNING "Cannot register forward timer!");
	    return result;
	  }
	
	PDEBUG("Chardev: Modul startet: Mayor number: %i\n",chardev_major);
	return 0;
}

/*
Cleaning the Module on exit call.
*/
static void chardev_exit(void){
	dev_t devno = MKDEV(chardev_major, MINOR_START);
	mutex_destroy(&myMutex);
	cdev_del(&my_cdev);	//deleting the char_device
	
	//This function will unregister a range of count device numbers, starting with devno.
	unregister_chrdev_region(devno,DEVICE_COUNT); // 
	PDEBUG("Chardev: Modul cleared\n");
}

// module init and exit calls
module_init(chardev_init);
module_exit(chardev_exit);
