#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************
 * MAC queue configuration
 */
#define ETH_TXBUFNB                   8    /* The number of descriptors sent by the MAC  */

#define ETH_RXBUFNB                   16    /* Number of MAC received descriptors  */

//#ifndef ETH_MAX_PACKET_SIZE
//#define ETH_RX_BUF_SZE                1520  /* MAC receive buffer length, an integer multiple of 4 */
//#define ETH_TX_BUF_SZE                1520  /* MAC send buffer length, an integer multiple of 4 */
//#else
#define ETH_RX_BUF_SIZE                384//512 //ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SIZE                384 //ETH_MAX_PACKET_SIZE
//#endif


#ifdef __cplusplus
}
#endif
#endif
