
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static struct input_dev *uk_dev;
static char *usb_buf;
static dma_addr_t usb_buf_phys;
static int len;
static struct urb *uk_urb;

static void usbmouse_as_key_irq(struct urb *urb)
{
    static unsigned char pre_val;
#if 0
    int i;
    static int cnt = 0;
    printk("data cnt %d: ", ++cnt);
    for(i = 0; i < len; i++)
    {
        printk("%02x ", usb_buf[i]);
    }
    printk("\n");
    usb_submit_urb(uk_urb, GFP_KERNEL);
#else
    if((pre_val & (1 << 0)) != (usb_buf[0] & (1 << 0))){
        input_event(uk_dev, EV_KEY, KEY_L, (usb_buf[0] & (1 << 0)) ? 1 : 0);
        input_sync(uk_dev);
    } 
    if((pre_val & (1 << 1)) != (usb_buf[0] & (1 << 1))){
        input_event(uk_dev, EV_KEY, KEY_S, (usb_buf[0] & (1 << 1)) ? 1 : 0);
        input_sync(uk_dev);
    }
    if((pre_val & (1 << 2)) != (usb_buf[0] & (1 << 2))){
        input_event(uk_dev, EV_KEY, KEY_ENTER, (usb_buf[0] & (1 << 2)) ? 1 : 0);
        input_sync(uk_dev);
    }
#endif
}

static int usbmouse_as_key_probe (struct usb_interface *intf,
          const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev(intf);
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
    int pipe;

    printk(KERN_INFO "find usb device\n");

    
    printk("bcdUSB = %x \n", dev->descriptor.bcdUSB);
    printk("VID = %x \n", dev->descriptor.idVendor);
    printk("PID = %x \n", dev->descriptor.iProduct);

    interface = intf->cur_altsetting;
    endpoint = &interface->endpoint[0].desc;

    /*1、分配一个input_dev*/
    uk_dev = input_allocate_device();
    /*2、设置*/
    /*2.1、能产生那类事件*/
    set_bit(EV_KEY, uk_dev->evbit);
    set_bit(EV_PWR, uk_dev->evbit);
    /*2.1、能产生哪些事件*/
    set_bit(KEY_L, uk_dev->keybit);
    set_bit(KEY_S, uk_dev->keybit);
    set_bit(KEY_ENTER, uk_dev->keybit);
    /*3、注册*/
    input_register_device(uk_dev);
    
    /*4、硬件相关操作*/
    /*数据传输3要素*/
    /*源:USB设备的某个端点*/
    pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);

    /*长度*/
    len = endpoint->wMaxPacketSize;
    
    /*目的 */
    usb_buf = usb_alloc_coherent(dev, len, GFP_ATOMIC, &usb_buf_phys);

    /*使用3要素*/
    /*分配usb request block*/
    uk_urb = usb_alloc_urb(0, GFP_KERNEL);
    /*使用3要素设置urb usb请求块，设备，源，目的，                 长度    完成函数                       usb控制器查询频率  */  
    //usb设备没有中断能力，但是usb主机控制器有中断能力，当usb主机查询到usb设备有数据时会中断主机，从而调用usbmouse_as_key_irq
	usb_fill_int_urb(uk_urb, dev, pipe, usb_buf, len, usbmouse_as_key_irq, NULL, endpoint->bInterval);
    
	uk_urb->transfer_dma = usb_buf_phys;
	uk_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    /*使用urb*/
    usb_submit_urb(uk_urb, GFP_KERNEL);
    return 0;
}

static void usbmouse_as_key_disconnect (struct usb_interface *intf)
{
    struct usb_device *dev = interface_to_usbdev(intf);

    usb_kill_urb(uk_urb);
    usb_free_urb(uk_urb);
    usb_free_coherent(dev, len, usb_buf, usb_buf_phys);

    
    input_unregister_device(uk_dev);
    input_free_device(uk_dev);
    printk(KERN_INFO "usb device disconnect\n");

}

static struct usb_device_id usbmouse_as_key_id_table[] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }	/* Terminating entry */

};

static struct usb_driver usbmouse_as_key_driver = {
	.name		= "usbmouse",
	.probe		= usbmouse_as_key_probe,
	.disconnect	= usbmouse_as_key_disconnect,
	.id_table	= usbmouse_as_key_id_table,
};

static int usbmouse_as_key_init(void)
{
	int retval = usb_register(&usbmouse_as_key_driver);
	if (retval != 0)
		printk("register usbmouse_as_key driver failed! \n");

	return retval;
}

static void usbmouse_as_key_exit(void)
{
	usb_deregister(&usbmouse_as_key_driver);
}


module_init(usbmouse_as_key_init);
module_exit(usbmouse_as_key_exit);

MODULE_LICENSE("GPL");

