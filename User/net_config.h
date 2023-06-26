#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************
 * MAC queue configuration
 */
#define ETH_TXBUFNB                   2    /* The number of descriptors sent by the MAC  */

#define ETH_RXBUFNB                   4    /* Number of MAC received descriptors  */

#ifndef ETH_MAX_PACKET_SIZE
#define ETH_RX_BUF_SZE                1520  /* MAC receive buffer length, an integer multiple of 4 */
#define ETH_TX_BUF_SZE                1520  /* MAC send buffer length, an integer multiple of 4 */
#else
#define ETH_RX_BUF_SZE                ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SZE                ETH_MAX_PACKET_SIZE
#endif


#ifdef __cplusplus
}
#endif
#endif
