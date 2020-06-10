/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Natale Patriciello <natale.patriciello@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "tcp-congestion-ops.h"
#include "tcp-socket-base.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpCongestionOps");

NS_OBJECT_ENSURE_REGISTERED (TcpCongestionOps);

TypeId
TcpCongestionOps::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpCongestionOps")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TcpCongestionOps::TcpCongestionOps () : Object ()
{
}

TcpCongestionOps::TcpCongestionOps (const TcpCongestionOps &other) : Object (other)
{
}

TcpCongestionOps::~TcpCongestionOps ()
{
}
/*CHANGED -- The existing/default implementation of TCP New Reno in ns-3 follows
RFC standards which increases cwnd more conservatively than Linux kernel TCP New Reno.
So slow start and congestion avoidance algorithms of TCP New Reno in ns3 is modified here to 
increase cwnd based on the number of bytes being acknowledged by each 
arriving ACK, rather than by the number of ACKs that arrive.
 */


// RENO

NS_OBJECT_ENSURE_REGISTERED (TcpNewReno);

TypeId
TcpNewReno::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpNewReno")
    .SetParent<TcpCongestionOps> ()
    .SetGroupName ("Internet")
    .AddConstructor<TcpNewReno> ()
  ;
  return tid;
}

TcpNewReno::TcpNewReno (void) : TcpCongestionOps ()
{
  NS_LOG_FUNCTION (this);
}

TcpNewReno::TcpNewReno (const TcpNewReno& sock)
  : TcpCongestionOps (sock)
{
  NS_LOG_FUNCTION (this);
}

TcpNewReno::~TcpNewReno (void)
{
}
/*CHANGED -- In slow start phase, on each incoming ACK at the TCP sender side cwnd 
is increased by the number of previously unacknowledged bytes ACKed by the
incoming acknowledgment. In contrast, in existing/default ns-3 New Reno version, cwnd is 
increased one segment.Our implementation is similar to Linux kernel implementation.
*/
uint32_t
TcpNewReno::SlowStart (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  if (segmentsAcked >= 1)
    {
      uint32_t sndCwnd = tcb->m_cWnd;
      tcb->m_cWnd = std::min<uint32_t> ((sndCwnd + (segmentsAcked * tcb->m_segmentSize)), tcb->m_ssThresh);
      NS_LOG_INFO ("In SlowStart, updated to cwnd " << tcb->m_cWnd << " ssthresh " << tcb->m_ssThresh);
      NS_ASSERT_MSG (segmentsAcked >= ((tcb->m_cWnd - sndCwnd) / tcb->m_segmentSize), "The leftover ACKs to adjust cwnd in congestion avoidance mode must be positive or zero");
      return segmentsAcked - ((tcb->m_cWnd - sndCwnd) / tcb->m_segmentSize);
    }

  return 0;
}
/*CHANGED -- In congestion avoidance phase, the number of bytes that have been ACKed at
the TCP sender side are stored in a 'bytes_acked' variable in the TCP control
block. When 'bytes_acked' becomes greater than or equal to the value of the
cwnd, 'bytes_acked' is reduced by the value of cwnd. Next, cwnd is incremented
by a full-sized segment (SMSS). Whereas in existing/default ns-3 New Reno, cwnd is increased
by (1/cwnd) with a rounding off due to type casting into int.Our implementation is similar to Linux kernel implementation.
*/
void
TcpNewReno::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  uint32_t cWndInPacket = tcb->GetCwndInSegments ();
  NS_LOG_DEBUG ("cWndInPacket in segments " << cWndInPacket << " m_cWndCntInPacket " << m_cWndCntInPacket << " segments acked " << segmentsAcked);
  if (m_cWndCntInPacket >= cWndInPacket)
    {
      m_cWndCntInPacket = 0;
      tcb->m_cWnd += tcb->m_segmentSize;
      NS_LOG_DEBUG ("Adding 1 segment to m_cWnd");
    }

  m_cWndCntInPacket += segmentsAcked;
  NS_LOG_DEBUG ("Adding 1 segment to m_cWndCntInPacket");
  if (m_cWndCntInPacket >= cWndInPacket)
    {
      uint32_t delta = m_cWndCntInPacket / cWndInPacket;

      m_cWndCntInPacket -= delta * cWndInPacket;
      tcb->m_cWnd += delta * tcb->m_segmentSize;
      NS_LOG_DEBUG ("Subtracting delta * w from m_cWndCntInPacket " << delta * cWndInPacket);
    }
  NS_LOG_DEBUG ("At end of CongestionAvoidance(), m_cWnd: " << tcb->m_cWnd << " m_cWndCntInPacket: " << m_cWndCntInPacket);
}

void
TcpNewReno::IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  // Linux tcp_in_slow_start() condition
  if (tcb->m_cWnd < tcb->m_ssThresh)
    {
      NS_LOG_DEBUG ("In slow start, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh);
      segmentsAcked = SlowStart (tcb, segmentsAcked);
    }
  else
    {
      NS_LOG_DEBUG ("In cong. avoidance, m_cWnd " << tcb->m_cWnd << " m_ssThresh " << tcb->m_ssThresh);
      CongestionAvoidance (tcb, segmentsAcked);
    }
}

std::string
TcpNewReno::GetName () const
{
  return "TcpNewReno";
}

uint32_t
TcpNewReno::GetSsThresh (Ptr<const TcpSocketState> state,
                           uint32_t bytesInFlight)
{
  NS_LOG_FUNCTION (this << state << bytesInFlight);

  // In Linux, it is written as:  return max(tp->snd_cwnd >> 1U, 2U);
  return std::max<uint32_t> (2 * state->m_segmentSize, state->m_cWnd / 2);
}

Ptr<TcpCongestionOps>
TcpNewReno::Fork ()
{
  return CopyObject<TcpNewReno> (this);
}

} // namespace ns3

