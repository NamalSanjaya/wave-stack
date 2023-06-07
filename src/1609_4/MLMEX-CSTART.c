// Required header files for the Linux kernel module development.
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <linux/ip.h>

// Declare the module license and description.it is set to "GPL".
MODULE_LICENSE("GPL");

// variables used in the module.
int frequency = 585000000;
int TimeSlot = 1;
int ImmediateAccess = 255;
char frq_str[10];
char source_addr[] = "192.168.1";
char dest_addr[] = "192.168.1";
char data[] = "Hello World! Voice from Kernel";

// Declares a pointer to a net_device, It will be used to reference the TUN device.
static struct net_device *tun_dev;

// The function send_to_tun is responsible for sending the data to the TUN device.
static int send_to_tun(char *str, int length, char *tun_name)
{
    struct sk_buff *skb;
    struct iphdr *iph;
    struct net_device *dev;
    int ret = -ENODEV;

    dev = dev_get_by_name(&init_net, tun_name);
    if (!dev) {
        printk(KERN_ERR "Could not find TUN device\n");
        return ret;
    }

    skb = alloc_skb(sizeof(struct iphdr) + length, GFP_KERNEL);
    if (!skb) {
        printk(KERN_ERR "Failed to allocate skb\n");
        goto out;
    }

    skb_reserve(skb, sizeof(struct iphdr));
    skb_put(skb, length);

    iph = ip_hdr(skb);
    iph->version = 4;
    iph->ihl = sizeof(struct iphdr) >> 2;
    iph->ttl = 64;
    iph->protocol = IPPROTO_RAW;
    iph->saddr = in_aton("192.168.200.1");
    iph->daddr = in_aton("10.0.0.2");
    iph->tot_len = htons(sizeof(struct iphdr) + length);

    memcpy(skb->data, str, length);

    skb->dev = dev;
    skb->pkt_type = PACKET_HOST;
    skb->protocol = htons(ETH_P_IP);

    ret = dev_queue_xmit(skb);
    if (ret < 0) {
        printk(KERN_ERR "Failed to transmit skb\n");
        kfree_skb(skb);
    }

out:
    dev_put(dev);
    return ret;
}

// Sending the frequency where data needs to be transmitted
static int MLMEX_CHSTART(int frequency, int TimeSlot, int ImmediateAccess) {
    tun_dev = dev_get_by_name(&init_net, "tun0");

    if (!tun_dev) {
        pr_err("Could not find TUN device\n");
        return -ENODEV;
    }
    
    printk(KERN_INFO "Channel Communication Starting\n");
    snprintf(frq_str, sizeof(frq_str), "%d", frequency);
    send_to_tun(frq_str, strlen(frq_str),"tun0");
    printk(KERN_INFO "Channel Communication Started\n");
    return 0;
}

// Sending the data
static int MA_UNITDATAX(char *source_addr,char *dest_addr,char *data,int frequency,int TimeSlot){
    tun_dev = dev_get_by_name(&init_net, "tun0");

    if (!tun_dev) {
        pr_err("Could not find TUN device\n");
        return -ENODEV;
    }
    
    printk(KERN_INFO "Channel Communication Starting\n");
    send_to_tun(data, strlen(data),"tun0");
    printk(KERN_INFO "Channel Communication Started\n");
    return 0;
}


// Define the module initialization
static int __init layer_initialization(void) {


    printk(KERN_INFO "Requesting MLME to provide Access to the chanel\n");
    MLMEX_CHSTART(frequency,TimeSlot,ImmediateAccess);
    printk(KERN_INFO "Requesting MLME to provide Access Success\n");
    
    printk(KERN_INFO "Requesting to start transfer MSDU\n");
    MA_UNITDATAX(source_addr,dest_addr,data,frequency,TimeSlot);
    printk(KERN_INFO "Transfered the MSDU\n");
    
    return 0;
}

// Exit function of the module
static void __exit layer_exit(void) {

    dev_put(tun_dev);
    printk(KERN_INFO "Layer Unloaded\n");
    
}

// Register the initialization and cleanup functions
module_init(layer_initialization);
module_exit(layer_exit);

