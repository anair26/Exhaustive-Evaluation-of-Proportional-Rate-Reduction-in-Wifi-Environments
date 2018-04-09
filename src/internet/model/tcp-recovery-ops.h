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
#ifndef TCPRECOVERYOPS_H
#define TCPRECOVERYOPS_H

#include "ns3/object.h"
#include "ns3/timer.h"
#include "ns3/tcp-socket-base.h"

namespace ns3 {

/**
 * \ingroup tcp
 * \defgroup recoveryOps Recovery Algorithms.
 *
 * The various recovery algorithms used in recovery phase of TCP.
 */

/**
 * \ingroup recoveryOps
 *
 * \brief recovery abstract class
 *
 * The design TBD
 *
 * \see DoRecovery
 */
class TcpRecoveryOps : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpRecoveryOps ();

  /**
   * \brief Copy constructor.
   * \param other object to copy.
   */
  TcpRecoveryOps (const TcpRecoveryOps &other);

  virtual ~TcpRecoveryOps ();

  /**
   * \brief Get the name of the recovery algorithm
   *
   * \return A string identifying the name
   */
  virtual std::string GetName () const = 0;

  /**
   * \brief TBD
   *
   * \param tcb internal congestion state
   * \param unAckDataCount total bytes of data unacknowledged
   * \param isSackEnabled
   * \param dupAckCount duplicate acknowldgement count
   * \param bytesInFlight total bytes in flight
   * \param lastDeliveredBytes bytes delivered in the last ACK
   */
  virtual void EnterRecovery (Ptr<TcpSocketState> tcb, uint32_t unAckDataCount,
                           bool isSackEnabled, uint32_t dupAckCount,
                           uint32_t bytesInFlight, uint32_t lastDeliveredBytes) = 0;

  /**
   * \brief TBD
   *
   * \param tcb internal congestion state
   * \param unAckDataCount total bytes of data unacknowledged
   * \param isSackEnabled
   * \param dupAckCount duplicate acknowldgement count
   * \param bytesInFlight total bytes in flight
   * \param lastDeliveredBytes bytes delivered in the last ACK
   */
  virtual void DoRecovery (Ptr<TcpSocketState> tcb, uint32_t unAckDataCount,
                           bool isSackEnabled, uint32_t dupAckCount,
                           uint32_t bytesInFlight, uint32_t lastDeliveredBytes) = 0;

  /**
   * \brief TBD
   *
   * \param tcb internal congestion state
   * \param bytesInFlight total bytes in flight
   */
  virtual void ExitRecovery (Ptr<TcpSocketState> tcb, uint32_t bytesInFlight) = 0;

  /**
   * \brief TBD
   *
   * \param bytesSent bytes sent
   */
  virtual void UpdateBytesSent (uint32_t bytesSent) = 0;

  /**
   * \brief Copy the recovery algorithm across socketTBD
   *
   * \return a pointer of the copied object
   */
  virtual Ptr<TcpRecoveryOps> Fork () = 0;
};

/**
 * \brief The Classic recovery implementation TBD
 *
 * \see DoRecovery
 */
class ClassicRecovery : public TcpRecoveryOps
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  ClassicRecovery ();

  /**
   * \brief Copy constructor.
   * \param recovery object to copy.
   */
  ClassicRecovery (const ClassicRecovery& recovery);

  ~ClassicRecovery ();

  std::string GetName () const;

  virtual void EnterRecovery (Ptr<TcpSocketState> tcb, uint32_t unAckDataCount,
                           bool isSackEnabled, uint32_t dupAckCount,
                           uint32_t bytesInFlight, uint32_t lastDeliveredBytes);

  virtual void DoRecovery (Ptr<TcpSocketState> tcb, uint32_t unAckDataCount,
                           bool isSackEnabled, uint32_t dupAckCount,
                           uint32_t bytesInFlight, uint32_t lastDeliveredBytes);

  virtual void ExitRecovery (Ptr<TcpSocketState> tcb, uint32_t bytesInFlight);

  virtual void UpdateBytesSent (uint32_t bytesSent);

  virtual Ptr<TcpRecoveryOps> Fork ();
};

} // namespace ns3

#endif // TCPRECOVERYOPS_H
