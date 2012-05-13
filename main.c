#include <stdio.h>

#include "debug.h"
#include "type.h"

#include "lpc214x.h"

#include "network.h"

#include "lcd.h"
#include "uip.h"
#include "uip_arp.h"
#include "timer.h"
#include "dhcpc.h"

#include "clock.h"

#include "webserver.h"

#define ETH_BUF		((struct uip_eth_hdr *)&uip_buf[0])
#define MY_MAC_ADDR	{ 0x00, 0xf8, 0xc1, 0xd8, 0xc7, 0xa6} 
#define MSG_UIP_LOG	MSG_DEBUG

extern u16_t uip_slen;

DEFINE_pmesg_level(MSG_INFO);

void uip_log(char *m)
{
    pmesg(MSG_UIP_LOG, "uIP log message: %s\n", m);
}

void pmesg_hex(int level, uint8_t *buf, unsigned int len);

void pmesg_hex(int level, uint8_t *buf, unsigned int len) 
{
    unsigned int i;
    for(i = 0; i < len; i++) {
	if((i % 8) == 0 && i != 0) {
	    pmesg(level, "|");
	}
	if((i % 32) == 0 && i != 0) {
	    pmesg(level, "\n");
	}

	pmesg(level, " %.2x", buf[i]);
    }
    pmesg(level, "\n");
}

void dhcpc_configured(const struct dhcpc_state *s) {
    char ipmsg[20] = {0};
    uip_sethostaddr(s->ipaddr);
    uip_setdraddr(s->default_router);
    uip_setnetmask(s->netmask);
  
    pmesg(MSG_INFO, "- Setting IP to: `%d.%d.%d.%d'\n", 
	    uip_ipaddr1(s->ipaddr), 
	    uip_ipaddr2(s->ipaddr), 
	    uip_ipaddr3(s->ipaddr), 
	    uip_ipaddr4(s->ipaddr));
    pmesg(MSG_INFO, "- Setting default router IP to: `%d.%d.%d.%d'\n", 
	    uip_ipaddr1(s->default_router), 
	    uip_ipaddr2(s->default_router), 
	    uip_ipaddr3(s->default_router), 
	    uip_ipaddr4(s->default_router));

    pmesg(MSG_INFO,"*~*~*~*DHCPC CONFIGURED*~*~*~*\n\n\n");
    sprintf(ipmsg, "IP = %d.%d.%d.%d",
	    ((u8_t*)s->ipaddr)[0],
	    ((u8_t*)s->ipaddr)[1],
	    ((u8_t*)s->ipaddr)[2],
	    ((u8_t*)s->ipaddr)[3]);

    lcdClearScreen(); 
    lcdPrintString(ipmsg);
}

int main(void)
{
    unsigned int i;
    uint64_t j;
    struct uip_eth_addr macaddr = { 
	.addr = MY_MAC_ADDR
    };

    uip_ipaddr_t ipaddr;
    struct timer periodic_timer, arp_timer;

    timer_set(&periodic_timer, CLOCK_SECOND * 1);
    timer_set(&arp_timer, CLOCK_SECOND * 1);

    VPBDIV = 0x02;

    fopen("uart0", "w");

    lcdInit();
    clock_init();

    pmesg(MSG_INFO, "- Started Uart\n");

    pmesg(MSG_INFO, "- Starting Network...");
    network_init();
    pmesg(MSG_INFO, "Done\n");

    pmesg(MSG_INFO, "- Starting uIP...");
    uip_init();
    pmesg(MSG_INFO, "Done\n");

    uip_setethaddr(macaddr);
    network_set_mac((uint8_t *)&(macaddr.addr));
    pmesg(MSG_INFO, 
	    "- Setting MAC address to `%.2x:%.2x:%.2x:%.2x:%.2x:%.2x'\n", 
	    macaddr.addr[0], macaddr.addr[1], macaddr.addr[2], 
	    macaddr.addr[3], macaddr.addr[4],macaddr.addr[5]);
    dhcpc_init(macaddr.addr, sizeof(macaddr.addr));
    httpd_init();

    while(1) {
	uip_len = network_read(uip_buf);
	if(j++ % 1000 == 0) {
	    pmesg(MSG_DEBUG, "loop %ld\n", j);
	}
	if(uip_len > 0) 
	{
	    pmesg(MSG_DEBUG, "Got packet (len == %d)\n", uip_len);
	    pmesg_hex(MSG_DEBUG_MORE, uip_buf, uip_len);

	    if(ETH_BUF->type == htons(UIP_ETHTYPE_IP)) 
	    {
		pmesg(MSG_DEBUG, "Type: IP\n");
		uip_arp_ipin();
		uip_input();
		/* If the above function invocation resulted in data that
		   should be sent out on the network, the global variable
		   uip_len is set to a value > 0. */
		if(uip_len > 0) {
		    pmesg(MSG_DEBUG, "Sending response... (len == %d)\n", uip_len);
		    pmesg_hex(MSG_DEBUG_MORE, uip_buf, uip_len);

		    uip_arp_out();
		    network_send(uip_buf, uip_len);
		}
	    } 
	    else if(ETH_BUF->type == htons(UIP_ETHTYPE_ARP)) 
	    {
		pmesg(MSG_DEBUG, "Type: ARP\n");
		uip_arp_arpin();
		/* If the above function invocation resulted in data that
		   should be sent out on the network, the global variable
		   uip_len is set to a value > 0. */
		if(uip_len > 0) {
		    network_send(uip_buf, uip_len);
		}
	    }
	}
#if 1	
	else if(timer_expired(&periodic_timer)) 
	{
	    //pmesg(MSG_DEBUG, "Timer expired: periodic timer (%d)\n", periodic_timer.start);
	    timer_reset(&periodic_timer);
	    

   	    for(i = 0; i < UIP_UDP_CONNS; i++) {
	    uip_udp_periodic(i);
	    /*  If the above function invocation resulted in data that
		should be sent out on the network, the global variable
		uip_len is set to a value > 0. */
	    if(uip_len > 0) {
                uip_arp_out();
		network_send(uip_buf, uip_len);
	    }
	}

            for(i = 0; i < UIP_CONNS; i++) 
	    {
                uip_periodic(i);
		/* If the above function invocation resulted in data that
		   should be sent out on the network, the global variable
		   uip_len is set to a value > 0. */
		if(uip_len > 0) 
		{
		    uip_arp_out();
		    network_send(uip_buf, uip_len);
		}
	    }
	}

	/* Call the ARP timer function every 10 seconds. */
	if(timer_expired(&arp_timer)) 
	{
	    //pmesg(MSG_DEBUG, "Timer expired: arp timer (%d)\n", arp_timer.start);
	    timer_reset(&arp_timer);
	    uip_arp_timer();
	}
#endif
    } // while(1)

    return 0;
}
