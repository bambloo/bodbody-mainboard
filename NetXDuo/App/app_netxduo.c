/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_netxduo.c
 * @author  MCD Application Team
 * @brief   NetXDuo applicative file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* USER CODE END Header */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
#include "nxd_dhcp_client.h"
/* USER CODE BEGIN Includes */
#include "memory.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD      NxAppThread;
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
TX_SEMAPHORE   DHCPSemaphore;
NX_DHCP        DHCPClient;
/* USER CODE BEGIN PV */
NX_PACKET_POOL DHCPPacketPool;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID nx_app_thread_entry (ULONG thread_input);
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  CHAR *pointer;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END MX_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN 0 */
#if 0
  /* USER CODE END 0 */

  /* Initialize the NetXDuo system. */
  nx_system_initialize();

    /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation,
   * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
   */
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

    /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

   /* Create the main NX_IP instance */
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

    /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */

  /* USER CODE BEGIN ARP_Protocol_Initialization */
#endif
  nx_system_initialize();

  if (tx_byte_allocate((TX_BYTE_POOL *)memory_pool_dma(), (VOID **)&pointer,
                       NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
    return TX_POOL_ERROR;
  }

  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer,
                              NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS) {
    return NX_POOL_ERROR;
  }

  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) !=
      TX_SUCCESS) {
    return TX_POOL_ERROR;
  }

  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS,
                     NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver, pointer,
                     Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS) {
    return NX_NOT_SUCCESSFUL;
  }

  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) !=
      TX_SUCCESS) {
    return TX_POOL_ERROR;
  }
  /* USER CODE END ARP_Protocol_Initialization */

  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */

  /* USER CODE BEGIN ICMP_Protocol_Initialization */

  /* USER CODE END ICMP_Protocol_Initialization */

  ret = nx_icmp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable TCP Protocol */

  /* USER CODE BEGIN TCP_Protocol_Initialization */

  /* USER CODE END TCP_Protocol_Initialization */

  ret = nx_tcp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the UDP protocol required for  DHCP communication */

  /* USER CODE BEGIN UDP_Protocol_Initialization */

  /* USER CODE END UDP_Protocol_Initialization */

  ret = nx_udp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

   /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Create the DHCP client */

  /* USER CODE BEGIN DHCP_Protocol_Initialization */

  /* USER CODE END DHCP_Protocol_Initialization */

  ret = nx_dhcp_create(&DHCPClient, &NetXDuoEthIpInstance, "DHCP Client");

  if (ret != NX_SUCCESS)
  {
    return NX_DHCP_ERROR;
  }

  /* set DHCP notification callback  */
  tx_semaphore_create(&DHCPSemaphore, "DHCP Semaphore", 0);

  /* USER CODE BEGIN MX_NetXDuo_Init */
  if (tx_byte_allocate((TX_BYTE_POOL *)memory_pool_dma(), (VOID **)&pointer,
                       NX_DHCP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
    return TX_POOL_ERROR;
  }

  ret = nx_packet_pool_create(&DHCPPacketPool, "DHCP Packet Pool", NX_DHCP_PACKET_PAYLOAD, pointer,
                              NX_DHCP_PACKET_POOL_SIZE);
  if (ret != NX_SUCCESS) {
    return NX_DHCP_ERROR;
  }

  ret = nx_dhcp_packet_pool_set(&DHCPClient, &DHCPPacketPool);
  if (ret != NX_SUCCESS) {
    return NX_DHCP_ERROR;
  }
  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/**
* @brief  ip address change callback.
* @param ip_instance: NX_IP instance
* @param ptr: user data
* @retval none
*/
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)
{
  /* USER CODE BEGIN ip_address_change_notify_callback */
  tx_semaphore_put(&DHCPSemaphore);
  /* USER CODE END ip_address_change_notify_callback */
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_thread_entry (ULONG thread_input)
{
  /* USER CODE BEGIN Nx_App_Thread_Entry 0 */

  /* USER CODE END Nx_App_Thread_Entry 0 */

  UINT ret = NX_SUCCESS;

  /* USER CODE BEGIN Nx_App_Thread_Entry 1 */

  /* USER CODE END Nx_App_Thread_Entry 1 */

  /* register the IP address change callback */
  ret = nx_ip_address_change_notify(&NetXDuoEthIpInstance, ip_address_change_notify_callback, NULL);
  if (ret != NX_SUCCESS)
  {
    /* USER CODE BEGIN IP address change callback error */

    /* USER CODE END IP address change callback error */
  }

  /* start the DHCP client */
  ret = nx_dhcp_start(&DHCPClient);
  if (ret != NX_SUCCESS)
  {
    /* USER CODE BEGIN DHCP client start error */

    /* USER CODE END DHCP client start error */
  }
  printf("Looking for DHCP server ..\n");
  /* wait until an IP address is ready */
  if(tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    /* USER CODE BEGIN DHCPSemaphore get error */

    /* USER CODE END DHCPSemaphore get error */
  }

  /* USER CODE BEGIN Nx_App_Thread_Entry 2 */
  NX_UDP_SOCKET udp_socket;
  UINT status = nx_udp_socket_create(&NetXDuoEthIpInstance, &udp_socket, "Test udp_socket",
                                     NX_IP_NORMAL, NX_DONT_FRAGMENT, NX_IP_TIME_TO_LIVE, 0x200);
  if (status != NX_SUCCESS) {
    printf("Error creating socket: %d\n", status);
    return;
  }
  status = nx_udp_socket_bind(&udp_socket, 6000, NX_WAIT_FOREVER);
  if (status != NX_SUCCESS) {
    printf("Error binding socket: %d\n", status);
    return;
  }
  while (1) {
    tx_thread_relinquish();
    ULONG target_ip = IP_ADDRESS(192, 168, 1, 8);
    USHORT target_port = 7782;

    NX_PACKET *packet_ptr;
    status = nx_packet_allocate(&NxAppPool, &packet_ptr, NX_UDP_PACKET, NX_WAIT_FOREVER);
    if (status) {
      printf("allocate packet error: %d\n", status);
      continue;
    }

    UCHAR message[] = "Hello from NetX Duo serhjgpsoehrgpsiouerhgpioseruhgsiopuerhgosiperughsopierughspoierughspeirughspieruhgspieruhgspierughsioepruhgsoiperughsioperughspeiorughsioperuhgspierughspieurhgpsieurhgpseiurhgpseurhgpsieruhgpsieurhgpsierughspieruhgpsieruhgspeurgh!";
    status =
        nx_packet_data_append(packet_ptr, message, sizeof(message), &NxAppPool, NX_WAIT_FOREVER);
    if (status) {
      printf("Error appending data to packet: %d\n", status);
      nx_packet_release(packet_ptr);
      continue;
    }
    status = nx_udp_socket_send(&udp_socket, packet_ptr, target_ip, target_port);
    if (status) {
      printf("Error sending packet: %d\n", status);
      nx_packet_release(packet_ptr);
      continue;
    }
  }
  /* USER CODE END Nx_App_Thread_Entry 2 */

}
/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
