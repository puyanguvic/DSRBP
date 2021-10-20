# DSR
Delay-guaranteed Scheduling and Routing (DSR) protocol integrates information from the network and link layers to accurately calculate the shortest delay required for transmission and provide on-time data transfer services.

## Tutorials
1. Nice handbook by Cisco to understand OSPF
https://www.ciscopress.com/articles/article.asp?p=2294214
2. Nice handbook by Cisco to understand Dynamic Routing
https://www.ciscopress.com/articles/article.asp?p=2180210

3. Get familiar with NS-3:
https://www.nsnam.org/doxygen/


## Getting started

1. System setup:
   - Recent Linux operating system (e.g., Ubuntu 18+)
   - Recent ns3 (e.g., ns3-33)
   - Download ns3 at: https://www.nsnam.org/docs/tutorial/html/getting-started.html
   
2. Run tests:
   clone the folder into the scratch folder of your ns3, and run it by waf
   ```
   cd ns-3.33/scratch
   git clone https://github.com/puyanguvic/dsr.git
   cd ..
   ./waf --run dsr-sim
   ```

## model design

1. dsr-apps module:
   - Create Udpsocket
   - add timestamp to packets
   - tracke the delay in the receiver side
2. timestamp-tag module:
   - a custemized bytes tag attached to packets
3. dsr-queue-discs
   - based on the FeCodle queue
   - maintain three different priority queues for different services
   - head drop for traffic control
4. dsr-routing
   - based on OSPF
   - generate routing tables based on bandwidth and delay, respecitively.

## Updating now!!!
