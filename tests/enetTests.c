#include "../enet.h"
#include <error.h>
#include <stdio.h>
#include <string.h>

ENetEventType waitEvents(ENetHost* h, unsigned timeout, const char* hostName){
  ENetEvent event;
  ENetEventType type = ENET_EVENT_TYPE_NONE;
  printf("-> %s for %d\n",hostName, timeout);
  /* Wait up to 1000 milliseconds for an event. */
  while (enet_host_service (h, & event, timeout) > 0)
  {
    type = event.type;
      switch (event.type)
      {
      case ENET_EVENT_TYPE_CONNECT:
          printf ("A new client connected to %s from %x:%u.\n",hostName,
                  event.peer -> address.host,
                  event.peer -> address.port);
          /* Store any relevant client information here. */
          event.peer -> data = "client";
          break;
      case ENET_EVENT_TYPE_RECEIVE:
          printf ("%s received a packet of length %u containing %s was received from %s on channel %u.\n",hostName,
                  event.packet -> dataLength,
                  (char*)event.packet -> data,
                  (char*)event.peer -> data,
                  event.channelID);
          /* Clean up the packet now that we're done using it. */
          enet_packet_destroy (event.packet);

          break;
      case ENET_EVENT_TYPE_DISCONNECT:
          printf ("%s received disconnection event from %s\n", hostName, (char*)event.peer -> data);
          /* Reset the peer's client information. */
          event.peer -> data = NULL;
      }
      return type;
  }
  return type;
}

int main (int argc, char ** argv){
  printf("-- enetTests started --\n");
  if (enet_initialize () != 0){
    fprintf (stderr, "An error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }
  ENetAddress address;
  ENetHost * server;
  /* Bind the server to the default localhost.     */
  /* A specific host address can be specified by   */
  /* enet_address_set_host (& address, "x.x.x.x"); */
  address.host = ENET_HOST_ANY;
  /* Bind the server to port 1234. */
  address.port = 1234;

  server = enet_host_create (& address /* the address to bind the server host to */,
                               32      /* allow up to 32 clients and/or outgoing connections */,
                                2      /* allow up to 2 channels to be used, 0 and 1 */,
                                0      /* assume any amount of incoming bandwidth */,
                                0      /* assume any amount of outgoing bandwidth */);

  if (server == NULL){
      fprintf (stderr, "An error occurred while trying to create an ENet server host.\n");
      exit (EXIT_FAILURE);
  }

  ENetHost * client;
  client = enet_host_create (NULL /* create a client host */,
              1 /* only allow 1 outgoing connection */,
              2 /* allow up 2 channels to be used, 0 and 1 */,
              57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
              14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
  if (client == NULL)
  {
      fprintf (stderr, "An error occurred while trying to create an ENet client host.\n");
      exit (EXIT_FAILURE);
  }


  ENetAddress dstaddress;
  ENetEvent event;
  ENetPeer *peer;

  /* Connect to some.server.net:1234. */
  enet_address_set_host (& dstaddress, "localhost");
  dstaddress.port = 1234;
  /* Initiate the connection, allocating the two channels 0 and 1. */
  peer = enet_host_connect (client, & dstaddress, 2, 0);
  if (peer == NULL){
     fprintf (stderr,  "No available peers for initiating an ENet connection.\n");
     exit (EXIT_FAILURE);
  }

  waitEvents(client, 100, "client send connect");

  waitEvents(server, 100, "server wait connect");

  /* Wait up to 5 seconds for the connection attempt to succeed. */
  if (enet_host_service (client, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT){
      printf("Connection to localhost:1234 succeeded\n");
  }else{
      /* Either the 5 seconds are up or a disconnect event was */
      /* received. Reset the peer in the event the 5 seconds   */
      /* had run out without any significant event.            */
      enet_peer_reset (peer);
      fprintf (stderr, "Connection to localhost:1234 failed\n");
      exit(EXIT_FAILURE);
  }

  /* Create a reliable packet of size 7 containing "packet\0" */
  ENetPacket * packet = enet_packet_create ("packet",
                                            strlen ("packet") + 1,
                                            ENET_PACKET_FLAG_RELIABLE);
  /* Extend the packet so and append the string "foo", so it now */
  /* contains "packetfoo\0"                                      */
  enet_packet_resize (packet, strlen ("packetfoo") + 1);
  strcpy (& packet -> data [strlen ("packet")], "foo");
  /* Send the packet to the peer over channel id 0. */
  /* One could also broadcast the packet by         */
  /* enet_host_broadcast (host, 0, packet);         */
  enet_peer_send (peer, 0, packet);
  /* One could just use enet_host_service() instead. */

  waitEvents(client, 100, "client send msg");

  waitEvents(server, 100, "server wait connect");
  waitEvents(server, 100, "server wait msg");


  enet_peer_disconnect (peer, 0);

  waitEvents(client, 100, "client send disconnect");
  waitEvents(server, 100, "server wait disconnect");

  if(waitEvents(client, 100, "client wait disconnect")!=ENET_EVENT_TYPE_DISCONNECT){
    /* We've arrived here, so the disconnect attempt didn't */
    /* succeed yet.  Force the connection down.             */
    enet_peer_reset (peer);
    fprintf(stderr, "received event is not disconnect, will reset the peer\n");
    exit(EXIT_FAILURE);
  }

  enet_host_destroy(client);
  enet_host_destroy(server);
  enet_deinitialize();
  printf("-- enetTests Finished --\n");
  return EXIT_SUCCESS;
}
