#include "lwip/opt.h"
#include "scream_rx.h"
#include "app_ethernet.h"
#include "main.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"


#define UDP_RX_THREAD_PRIO  ( tskIDLE_PRIORITY + 4 )

static struct netconn *conn;
static struct netbuf *buf;
static struct ip_addr *addr;
static unsigned short port;
/*-----------------------------------------------------------------------------------*/

#define SCREAM_BIND_PORT 4010


static void udp_rx_thread(void *arg)
{
    err_t err, recv_err;

    LWIP_UNUSED_ARG(arg);


    conn = netconn_new(NETCONN_UDP);
    if (conn!= NULL)
    {
        err = netconn_bind(conn, IP_ADDR_ANY, SCREAM_BIND_PORT);
        if (err == ERR_OK)
        {
            while (1)
            {
                recv_err = netconn_recv(conn, &buf);

                if (recv_err == ERR_OK)
                {
                    addr = netbuf_fromaddr(buf);
                    port = netbuf_fromport(buf);
                    //netconn_connect(conn, addr, port);
                    //buf->addr.addr = 0;
                    //netconn_send(conn,buf);

                    struct pbuf *p = buf->p;
                    on_scream_server_recv(p->payload, p->len);

                    netbuf_delete(buf);
                }
            }
        }
        else
        {
            netconn_delete(conn);
        }
    }
}
/*-----------------------------------------------------------------------------------*/
void udp_rx_init(void)
{
    sys_thread_new("udp_rx_thread", udp_rx_thread, NULL, DEFAULT_THREAD_STACKSIZE, UDP_RX_THREAD_PRIO );
}

#endif /* LWIP_NETCONN */
