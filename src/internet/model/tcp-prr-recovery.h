/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
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
 * Author: Viyom Mittal <viyommittal@gmail.com>
 *         Vivek Jain <jain.vivek.anand@gmail.com>
 *         Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 *
 */

#ifndef TcpPrrRecovery_H
#define TcpPrrRecovery_H

#include "ns3/tcp-recovery-ops.h"

namespace ns3 {

/**
 * \ingroup recoveryOps
 *
 * \brief An implementation of PRR TBD
 */
class PrrRecovery : public ClassicRecovery
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Create an unbound tcp socket.
   */
  PrrRecovery (void);

  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  PrrRecovery (const PrrRecovery& sock);
  virtual ~PrrRecovery (void);

  typedef enum {
    CRB,
    SSRB
  } ReductionBound_t;

  std::string GetName () const;

  virtual void EnterRecovery (Ptr<TcpSocketState> tcb, uint32_t unAckDataCount,
                           bool isSackEnabled, uint32_t dupAckCount,
                           uint32_t bytesInFlight, uint32_t lastDeliveredBytes);

  virtual void DoRecovery (Ptr<TcpSocketState> tcb, uint32_t unAckDataCount,
                           bool isSackEnabled, uint32_t dupAckCount,
                           uint32_t bytesInFlight, uint32_t lastDeliveredBytes);

  virtual void UpdateBytesSent (uint32_t bytesSent);

  virtual Ptr<TcpRecoveryOps> Fork ();

private:
  uint32_t m_prrDelivered;
  uint32_t m_prrOut;
  uint32_t m_recoveryFlightSize;
  ReductionBound_t m_reductionBoundMode;
};


} // namespace ns3

#endif // TcpPrrRecovery_H
