#include "chardev.h"

struct mutex myMutex;
static int chardev_major;
static int momentane_zeile_zeichen_anzahl;
static u64 zeit_stempel;
static int zeit_seit_letzte_gelessene_zeile;
static struct cdev my_cdev;
static int open = 0;
static int writeTrue = 0;

static int ret_val_time = DEF_RET_VAL_TIME ;
static int ret_val_number = DEF_RET_VAL_NUMBER;

int chardev_open(struct inode *inode, struct file *filp) {
	if(open ==0){
	PDEBUG("Chardev: OPEN\n");
	open =1;
	}else{
	  return -EBUSY;
	}
	return 0;
}

int chardev_release(struct inode *inode, struct file *filp) {
	// Nothing to do
	PDEBUG("Chardev: RELEASE\n");
	open = 0;
	return 0;
}

ssize_t chardev_read(struct file *filp
, char *buffer,//Buffer denn man fuelt
size_t buffer_length,//Die lange des buffers
loff_t *f_offset//Unser offset im file
){
	int ubergeben = 0;
	char ausgabe_buffer[BUFFER_SIZE] = { 0 };
	
	
	if(*f_offset != 0){ //zweiter read befehl
		return 0;
	}
	
	if (writeTrue == 1){
	  sprintf(ausgabe_buffer,"Eingegebene Zeichen: %i  Vergangene Zeit: %u\n",momentane_zeile_zeichen_anzahl,zeit_seit_letzte_gelessene_zeile);
	} else {
	  sprintf(ausgabe_buffer,"Eingegebene Zeichen: %i  Vergangene Zeit: %i\n",ret_val_number,ret_val_time);
	  }
	
	
	ubergeben = MINIMUM(strlen(ausgabe_buffer),buffer_length);
	
	mutex_lock(&myMutex);
	
	if(copy_to_user(buffer,ausgabe_buffer,ubergeben)){
		PDEBUG("Chardev: Daten wurden nicht gesendet");
	}
	else{
		PDEBUG("Chardev: Daten wurden gesendet");
	}
	
	PDEBUG("Chardev: Data mit der lange %d wird gelesen\n", ubergeben);
	
	*f_offset = ubergeben;
	mutex_unlock(&myMutex);
	return ubergeben;
	
}

ssize_t chardev_write(struct file *filp,const char *buffer,size_t buffer_length,loff_t *f_offset){
	
	u64 temp = 0;
	mutex_lock(&myMutex);
	momentane_zeile_zeichen_anzahl = buffer_length-1;
	PDEBUG("Chardev: Data mit der lange %d und wert %s wird gelessen\n", buffer_length,buffer);
	
	temp = (u64)get_jiffies_64();
	if(writeTrue==0){
	zeit_stempel = temp;
	}
	zeit_seit_letzte_gelessene_zeile = temp - zeit_stempel;
	zeit_seit_letzte_gelessene_zeile = zeit_seit_letzte_gelessene_zeile * 1000 / HZ;
	zeit_stempel = temp;
	PDEBUG("Chardev: Schreiben ist fertig\n");
	mutex_unlock(&myMutex);
	writeTrue = 1;
	return buffer_length;
}

struct file_operations fops = {
       .read = chardev_read,
       .write = chardev_write,
       .open = chardev_open,
       .release = chardev_release
    };

static int chardev_init(void){
	int result;
	dev_t dev = 0;
	
	mutex_init(&myMutex);
	
	PDEBUG("Chardev: Modul initialisation wurde gestartet\n");
	
	result = alloc_chrdev_region(&dev,MINOR_START,DEVICE_COUNT,"chardev");
	if (result < 0) {
		printk(KERN_WARNING "Cannot register Major number for timer!");
		return result;
	}
	
	chardev_major = MAJOR(dev);
	momentane_zeile_zeichen_anzahl = 0;
	zeit_seit_letzte_gelessene_zeile = 0;
	
	
	cdev_init(&my_cdev, &fops);
	
	result = cdev_add(&my_cdev, MKDEV(chardev_major, MINOR_START), 1);
	  if (result < 0) {
	    printk(KERN_WARNING "Cannot register forward timer!");
	    return result;
	  }
	
	PDEBUG("Chardev: Modul wurde mit der major numer %i gestartert\n",chardev_major);
	
	
	
	return 0;
}

static void chardev_exit(void){
	dev_t devno = MKDEV(chardev_major, MINOR_START);
	mutex_destroy(&myMutex);
	cdev_del(&my_cdev);	
	
	unregister_chrdev_region(devno,DEVICE_COUNT);
	PDEBUG("Chardev: Modul wurde aufgereumt\n");
}


module_init(chardev_init);
module_exit(chardev_exit);
