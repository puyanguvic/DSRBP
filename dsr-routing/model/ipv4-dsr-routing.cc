// -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-
//
// Copyright (c) 2008 University of Washington
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <vector>
#include <iomanip>
#include "ns3/names.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/net-device.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/boolean.h"
#include "ns3/node.h"
#include "ipv4-dsr-routing.h"
#include "dsr-route-manager.h"

/**
 * \author Pu Yang
*/
#include "ns3/dsr-tags-module.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Ipv4DSRRouting");

NS_OBJECT_ENSURE_REGISTERED (Ipv4DSRRouting);

TypeId 
Ipv4DSRRouting::GetTypeId (void)
{ 
  static TypeId tid = TypeId ("ns3::dsr-routing::Ipv4DSRRouting")
    .SetParent<Ipv4RoutingProtocol> ()
    .SetGroupName ("Dsr-routing")
    .AddConstructor<Ipv4DSRRouting> ()
    .AddAttribute ("RandomEcmpRouting",
                   "Set to true if packets are randomly routed among ECMP; set to false for using only one route consistently",
                   BooleanValue (false),
                   MakeBooleanAccessor (&Ipv4DSRRouting::m_randomEcmpRouting),
                   MakeBooleanChecker ())
    .AddAttribute ("RespondToInterfaceEvents",
                   "Set to true if you want to dynamically recompute the global routes upon Interface notification events (up/down, or add/remove address)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&Ipv4DSRRouting::m_respondToInterfaceEvents),
                   MakeBooleanChecker ())
  ;
  return tid;
}

Ipv4DSRRouting::Ipv4DSRRouting () 
  : m_randomEcmpRouting (false),
    m_respondToInterfaceEvents (false)
{
  NS_LOG_FUNCTION (this);

  m_rand = CreateObject<UniformRandomVariable> ();
}

Ipv4DSRRouting::~Ipv4DSRRouting ()
{
  NS_LOG_FUNCTION (this);
}

void 
Ipv4DSRRouting::AddHostRouteTo (Ipv4Address dest, 
                                   Ipv4Address nextHop, 
                                   uint32_t interface)
{
  NS_LOG_FUNCTION (this << dest << nextHop << interface);
  Ipv4DSRRoutingTableEntry *route = new Ipv4DSRRoutingTableEntry ();
  *route = Ipv4DSRRoutingTableEntry::CreateHostRouteTo (dest, nextHop, interface);
  m_hostRoutes.push_back (route);
}

void 
Ipv4DSRRouting::AddHostRouteTo (Ipv4Address dest, 
                                   uint32_t interface)
{
  NS_LOG_FUNCTION (this << dest << interface);
  Ipv4DSRRoutingTableEntry *route = new Ipv4DSRRoutingTableEntry ();
  *route = Ipv4DSRRoutingTableEntry::CreateHostRouteTo (dest, interface);
  m_hostRoutes.push_back (route);
}

/**
  * \author Pu Yang
  * \brief Add a host route to the global routing table with the distance 
  * between root and destination
  * \param dest The Ipv4Address destination for this route.
  * \param nextHop The next hop Ipv4Address
  * \param interface The network interface index used to send packets to the
  *  destination
  * \param distance The distance between root and destination
 */
void
Ipv4DSRRouting::AddHostRouteTo (Ipv4Address dest,
                       Ipv4Address nextHop,
                       uint32_t interface,
                       uint32_t distance)
{
  NS_LOG_FUNCTION (this << dest << nextHop << interface << distance);
  Ipv4DSRRoutingTableEntry *route = new Ipv4DSRRoutingTableEntry ();
  // std::cout << "add host route with the distance = " << distance;
  *route = Ipv4DSRRoutingTableEntry::CreateHostRouteTo(dest, nextHop, interface, distance);
  m_hostRoutes.push_back (route);
}



void l
Ipv4DSRRouting::AddNetworkRouteTo (Ipv4Address network, 
                                      Ipv4Mask networkMask, 
                                      Ipv4Address nextHop, 
                                      uint32_t interface)
{
  NS_LOG_FUNCTION (this << network << networkMask << nextHop << interface);
  Ipv4DSRRoutingTableEntry *route = new Ipv4DSRRoutingTableEntry ();
  *route = Ipv4DSRRoutingTableEntry::CreateNetworkRouteTo (network,
                                                        networkMask,
                                                        nextHop,
                                                        interface);
  m_networkRoutes.push_back (route);
}

void 
Ipv4DSRRouting::AddNetworkRouteTo (Ipv4Address network, 
                                      Ipv4Mask networkMask, 
                                      uint32_t interface)
{
  NS_LOG_FUNCTION (this << network << networkMask << interface);
  Ipv4DSRRoutingTableEntry *route = new Ipv4DSRRoutingTableEntry ();
  *route = Ipv4DSRRoutingTableEntry::CreateNetworkRouteTo (network,
                                                        networkMask,
                                                        interface);
  m_networkRoutes.push_back (route);
}

void 
Ipv4DSRRouting::AddASExternalRouteTo (Ipv4Address network, 
                                         Ipv4Mask networkMask,
                                         Ipv4Address nextHop,
                                         uint32_t interface)
{
  NS_LOG_FUNCTION (this << network << networkMask << nextHop << interface);
  Ipv4DSRRoutingTableEntry *route = new Ipv4DSRRoutingTableEntry ();
  *route = Ipv4DSRRoutingTableEntry::CreateNetworkRouteTo (network,
                                                        networkMask,
                                                        nextHop,
                                                        interface);
  m_ASexternalRoutes.push_back (route);
}


Ptr<Ipv4Route>
Ipv4DSRRouting::LookupDSRRoute (Ipv4Address dest, Ptr<NetDevice> oif)
{
  /**
   * 
   * \todo to rewrite the lookup functions to find the best route from the routing table.
   * the routing table in DSR routing is a SPF forest instead of a routing tree in global routing
  */





  NS_LOG_FUNCTION (this << dest << oif);
  NS_LOG_LOGIC ("Looking for route for destination " << dest);
  Ptr<Ipv4Route> rtentry = 0;
  // store all available routes that bring packets to their destination
  typedef std::vector<Ipv4DSRRoutingTableEntry*> RouteVec_t;
  RouteVec_t allRoutes;

  NS_LOG_LOGIC ("Number of m_hostRoutes = " << m_hostRoutes.size ());
  for (HostRoutesCI i = m_hostRoutes.begin (); 
       i != m_hostRoutes.end (); 
       i++) 
    {
      NS_ASSERT ((*i)->IsHost ());
      if ((*i)->GetDest () == dest)
        {
          if (oif != 0)
            {
              if (oif != m_ipv4->GetNetDevice ((*i)->GetInterface ()))
                {
                  NS_LOG_LOGIC ("Not on requested interface, skipping");
                  continue;
                }
            }
          allRoutes.push_back (*i);
          NS_LOG_LOGIC (allRoutes.size () << "Found dsr host route" << *i); 
        }
    }
  if (allRoutes.size () == 0) // if no host route is found
    {
      NS_LOG_LOGIC ("Number of m_networkRoutes" << m_networkRoutes.size ());
      for (NetworkRoutesI j = m_networkRoutes.begin (); 
           j != m_networkRoutes.end (); 
           j++) 
        {
          Ipv4Mask mask = (*j)->GetDestNetworkMask ();
          Ipv4Address entry = (*j)->GetDestNetwork ();
          if (mask.IsMatch (dest, entry)) 
            {
              if (oif != 0)
                {
                  if (oif != m_ipv4->GetNetDevice ((*j)->GetInterface ()))
                    {
                      NS_LOG_LOGIC ("Not on requested interface, skipping");
                      continue;
                    }
                }
              allRoutes.push_back (*j);
              NS_LOG_LOGIC (allRoutes.size () << "Found DSR network route" << *j);
            }
        }
    }
  if (allRoutes.size () == 0)  // consider external if no host/network found
    {
      for (ASExternalRoutesI k = m_ASexternalRoutes.begin ();
           k != m_ASexternalRoutes.end ();
           k++)
        {
          Ipv4Mask mask = (*k)->GetDestNetworkMask ();
          Ipv4Address entry = (*k)->GetDestNetwork ();
          if (mask.IsMatch (dest, entry))
            {
              NS_LOG_LOGIC ("Found external route" << *k);
              if (oif != 0)
                {
                  if (oif != m_ipv4->GetNetDevice ((*k)->GetInterface ()))
                    {
                      NS_LOG_LOGIC ("Not on requested interface, skipping");
                      continue;
                    }
                }
              allRoutes.push_back (*k);
              break;
            }
        }
    }
  if (allRoutes.size () > 0 ) // if route(s) is found
    {
      // pick up one of the routes uniformly at random if random
      // ECMP routing is enabled, or always select the first route
      // consistently if random ECMP routing is disabled
      uint32_t selectIndex;
      if (m_randomEcmpRouting)
        {
          selectIndex = m_rand->GetInteger (0, allRoutes.size ()-1);
        }
      else 
        {
          selectIndex = 0;
        }
      Ipv4DSRRoutingTableEntry* route = allRoutes.at (selectIndex);
      // std::cout << "the distance =" << route->GetDistance(); 
      // create a Ipv4Route object from the selected routing table entry
      rtentry = Create<Ipv4Route> ();
      rtentry->SetDestination (route->GetDest ());
      /// \todo handle multi-address case
      rtentry->SetSource (m_ipv4->GetAddress (route->GetInterface (), 0).GetLocal ());
      rtentry->SetGateway (route->GetGateway ());
      uint32_t interfaceIdx = route->GetInterface ();
      rtentry->SetOutputDevice (m_ipv4->GetNetDevice (interfaceIdx));
      /**
       * \author Pu Yang
       * \brief set the distance
      */
      // uint32_t distance = route->GetDistance();
      // rtentry->SetDistance (distance);
      return rtentry;
    }
  else 
    {
      return 0;
    }
}

uint32_t 
Ipv4DSRRouting::GetNRoutes (void) const
{
  NS_LOG_FUNCTION (this);
  uint32_t n = 0;
  n += m_hostRoutes.size ();
  n += m_networkRoutes.size ();
  n += m_ASexternalRoutes.size ();
  return n;
}

Ipv4DSRRoutingTableEntry *
Ipv4DSRRouting::GetRoute (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  if (index < m_hostRoutes.size ())
    {
      uint32_t tmp = 0;
      for (HostRoutesCI i = m_hostRoutes.begin (); 
           i != m_hostRoutes.end (); 
           i++) 
        {
          if (tmp  == index)
            {
              return *i;
            }
          tmp++;
        }
    }
  index -= m_hostRoutes.size ();
  uint32_t tmp = 0;
  if (index < m_networkRoutes.size ())
    {
      for (NetworkRoutesCI j = m_networkRoutes.begin (); 
           j != m_networkRoutes.end ();
           j++)
        {
          if (tmp == index)
            {
              return *j;
            }
          tmp++;
        }
    }
  index -= m_networkRoutes.size ();
  tmp = 0;
  for (ASExternalRoutesCI k = m_ASexternalRoutes.begin (); 
       k != m_ASexternalRoutes.end (); 
       k++) 
    {
      if (tmp == index)
        {
          return *k;
        }
      tmp++;
    }
  NS_ASSERT (false);
  // quiet compiler.
  return 0;
}
void 
Ipv4DSRRouting::RemoveRoute (uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  if (index < m_hostRoutes.size ())
    {
      uint32_t tmp = 0;
      for (HostRoutesI i = m_hostRoutes.begin (); 
           i != m_hostRoutes.end (); 
           i++) 
        {
          if (tmp  == index)
            {
              NS_LOG_LOGIC ("Removing route " << index << "; size = " << m_hostRoutes.size ());
              delete *i;
              m_hostRoutes.erase (i);
              NS_LOG_LOGIC ("Done removing host route " << index << "; host route remaining size = " << m_hostRoutes.size ());
              return;
            }
          tmp++;
        }
    }
  index -= m_hostRoutes.size ();
  uint32_t tmp = 0;
  for (NetworkRoutesI j = m_networkRoutes.begin (); 
       j != m_networkRoutes.end (); 
       j++) 
    {
      if (tmp == index)
        {
          NS_LOG_LOGIC ("Removing route " << index << "; size = " << m_networkRoutes.size ());
          delete *j;
          m_networkRoutes.erase (j);
          NS_LOG_LOGIC ("Done removing network route " << index << "; network route remaining size = " << m_networkRoutes.size ());
          return;
        }
      tmp++;
    }
  index -= m_networkRoutes.size ();
  tmp = 0;
  for (ASExternalRoutesI k = m_ASexternalRoutes.begin (); 
       k != m_ASexternalRoutes.end ();
       k++)
    {
      if (tmp == index)
        {
          NS_LOG_LOGIC ("Removing route " << index << "; size = " << m_ASexternalRoutes.size ());
          delete *k;
          m_ASexternalRoutes.erase (k);
          NS_LOG_LOGIC ("Done removing network route " << index << "; network route remaining size = " << m_networkRoutes.size ());
          return;
        }
      tmp++;
    }
  NS_ASSERT (false);
}

int64_t
Ipv4DSRRouting::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_rand->SetStream (stream);
  return 1;
}

void
Ipv4DSRRouting::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  for (HostRoutesI i = m_hostRoutes.begin (); 
       i != m_hostRoutes.end (); 
       i = m_hostRoutes.erase (i)) 
    {
      delete (*i);
    }
  for (NetworkRoutesI j = m_networkRoutes.begin (); 
       j != m_networkRoutes.end (); 
       j = m_networkRoutes.erase (j)) 
    {
      delete (*j);
    }
  for (ASExternalRoutesI l = m_ASexternalRoutes.begin (); 
       l != m_ASexternalRoutes.end ();
       l = m_ASexternalRoutes.erase (l))
    {
      delete (*l);
    }

  Ipv4RoutingProtocol::DoDispose ();
}

// Formatted like output of "route -n" command
void
Ipv4DSRRouting::PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit) const
{
  NS_LOG_FUNCTION (this << stream);
  std::ostream* os = stream->GetStream ();

  *os << "Node: " << m_ipv4->GetObject<Node> ()->GetId ()
      << ", Time: " << Now().As (unit)
      << ", Local time: " << m_ipv4->GetObject<Node> ()->GetLocalTime ().As (unit)
      << ", Ipv4DSRRouting table" << std::endl;

  if (GetNRoutes () > 0)
    {
      *os << "Destination     Gateway         Genmask         Flags Metric Ref    Use Iface" << std::endl;
      for (uint32_t j = 0; j < GetNRoutes (); j++)
        {
          /**
           * \author Pu Yang
           * \brief print the metric in routing table
          */
          std::ostringstream dest, gw, mask, flags, metric;
          Ipv4DSRRoutingTableEntry route = GetRoute (j);
          dest << route.GetDest ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << dest.str ();
          gw << route.GetGateway ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << gw.str ();
          mask << route.GetDestNetworkMask ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << mask.str ();
          flags << "U";
          if (route.IsHost ())
            {
              flags << "H";
            }
          else if (route.IsGateway ())
            {
              flags << "G";
            }
          *os << std::setiosflags (std::ios::left) << std::setw (6) << flags.str ();
          // // Metric not implemented
          // *os << "-" << "      ";
          /**
           * \author Pu Yang
          */
          metric << route.GetDistance ();
          *os << std::setiosflags (std::ios::left) <<std::setw(16) << metric.str();
          // std::cout << "the metric = " << route.GetDistance();

          
          // Ref ct not implemented
          *os << "-" << "      ";
          // Use not implemented
          *os << "-" << "   ";
          if (Names::FindName (m_ipv4->GetNetDevice (route.GetInterface ())) != "")
            {
              *os << Names::FindName (m_ipv4->GetNetDevice (route.GetInterface ()));
            }
          else
            {
              *os << route.GetInterface ();
            }
          *os << std::endl;
        }
    }
  *os << std::endl;
}



Ptr<Ipv4Route>
Ipv4DSRRouting::RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p << &header << oif << &sockerr);
//
// First, see if this is a multicast packet we have a route for.  If we
// have a route, then send the packet down each of the specified interfaces.
//
  if (header.GetDestination ().IsMulticast ())
    {
      NS_LOG_LOGIC ("Multicast destination-- returning false");
      return 0; // Let other routing protocols try to handle this
    }
//
// See if this is a unicast packet we have a route for.
//
  NS_LOG_LOGIC ("Unicast destination- looking up");
  Ptr<Ipv4Route> rtentry = LookupDSRRoute (header.GetDestination (), oif);
  if (rtentry)
    {
      sockerr = Socket::ERROR_NOTERROR;
      /**
       * \author Pu Yang 
       * \brief add the distance value the the packet tag
      */
      CostTag cost;
      p->RemovePacketTag (cost);
      // cost.SetCost (rtentry->GetDistance());
      // p->AddPacketTag (cost);
      // std::cout << "RO: the output cost = " << rtentry->GetDistance() << " the next hop = " << rtentry->GetOutputDevice() << std::endl;
      // std::cout << "the cost =" << cost.GetCost() << std::endl; 
      // std::cout << "the cost =" << rtentry->GetDistance() << std::endl;
    }
  else
    {
      sockerr = Socket::ERROR_NOROUTETOHOST;
    }
  return rtentry;
}



bool 
Ipv4DSRRouting::RouteInput  (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
                                UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                                LocalDeliverCallback lcb, ErrorCallback ecb)
{ 
  /**
   * \author Pu Yang
   * \brief make a copy of p, and process p_copy
  */
  Ptr <Packet> p_copy = p->Copy();
  NS_LOG_FUNCTION (this << p << header << header.GetSource () << header.GetDestination () << idev << &lcb << &ecb);
  // Check if input device supports IP
  NS_ASSERT (m_ipv4->GetInterfaceForDevice (idev) >= 0);
  uint32_t iif = m_ipv4->GetInterfaceForDevice (idev);

  if (m_ipv4->IsDestinationAddress (header.GetDestination (), iif))
    {
      if (!lcb.IsNull ())
        {
          NS_LOG_LOGIC ("Local delivery to " << header.GetDestination ());
          // std::cout << "Local delivery to " << header.GetDestination () << std::endl;
          lcb (p, header, iif);
          return true;
        }
      else
        {
          // The local delivery callback is null.  This may be a multicast
          // or broadcast packet, so return false so that another
          // multicast routing protocol can handle it.  It should be possible
          // to extend this to explicitly check whether it is a unicast
          // packet, and invoke the error callback if so
          // std::cout << "ERROR !!!!" << std::endl;
          return false;
        }
    }
  // Check if input device supports IP forwarding
  if (m_ipv4->IsForwarding (iif) == false)
    {
      NS_LOG_LOGIC ("Forwarding disabled for this interface");
      // std::cout << "RI: Forwarding disabled for this interface" << std::endl;
      ecb (p, header, Socket::ERROR_NOROUTETOHOST);
      return true;
    }
  // Next, try to find a route
  NS_LOG_LOGIC ("Unicast destination- looking up global route");
  Ptr<Ipv4Route> rtentry = LookupDSRRoute (header.GetDestination ());
  if (rtentry != 0)
    {
      /**
       * \author Pu Yang 
       * \brief add the distance value the the packet tag
      */
      // CostTag cost;
      // p_copy->RemovePacketTag (cost);
      // cost.SetCost (rtentry->GetDistance());
      // p_copy->AddPacketTag (cost);
      const Ptr <Packet> p_c = p_copy->Copy();
      // std::cout << "RI: the input cost = " << rtentry->GetDistance() << "the next hop = " << rtentry->GetOutputDevice() << std::endl;
      NS_LOG_LOGIC ("Found unicast destination- calling unicast callback");
      ucb (rtentry, p_c, header);
      return true; 
    }
  else
    {
      NS_LOG_LOGIC ("Did not find unicast destination- returning false");
      // std::cout << "RI: Did not find unicast destination" << std::endl;
      return false; // Let other routing protocols try to handle this
                    // route request.
    }
}
void 
Ipv4DSRRouting::NotifyInterfaceUp (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
      DSRRouteManager::DeleteDSRRoutes ();
      DSRRouteManager::BuildDSRRoutingDatabase ();
      DSRRouteManager::InitializeRoutes ();
    }
}

void 
Ipv4DSRRouting::NotifyInterfaceDown (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
      DSRRouteManager::DeleteDSRRoutes ();
      DSRRouteManager::BuildDSRRoutingDatabase ();
      DSRRouteManager::InitializeRoutes ();
    }
}

void 
Ipv4DSRRouting::NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address)
{
  NS_LOG_FUNCTION (this << interface << address);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
      DSRRouteManager::DeleteDSRRoutes ();
      DSRRouteManager::BuildDSRRoutingDatabase ();
      DSRRouteManager::InitializeRoutes ();
    }
}

void 
Ipv4DSRRouting::NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address)
{
  NS_LOG_FUNCTION (this << interface << address);
  if (m_respondToInterfaceEvents && Simulator::Now ().GetSeconds () > 0)  // avoid startup events
    {
      DSRRouteManager::DeleteDSRRoutes ();
      DSRRouteManager::BuildDSRRoutingDatabase ();
      DSRRouteManager::InitializeRoutes ();
    }
}

void 
Ipv4DSRRouting::SetIpv4 (Ptr<Ipv4> ipv4)
{
  NS_LOG_FUNCTION (this << ipv4);
  NS_ASSERT (m_ipv4 == 0 && ipv4 != 0);
  m_ipv4 = ipv4;
}


} // namespace ns3
