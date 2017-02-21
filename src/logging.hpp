/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2017, Regents of the University of California.
 *
 * This file is part of ChronoShare, a decentralized file sharing application over NDN.
 *
 * ChronoShare is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * ChronoShare is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received copies of the GNU General Public License along with
 * ChronoShare, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ChronoShare authors and contributors.
 */

#ifndef AUTONDN_LOGGING_HPP
#define AUTONDN_LOGGING_HPP

#include <ndn-cxx/util/logger.hpp>

namespace ndn {
namespace autondn {

#define _LOG_INIT(name) NDN_LOG_INIT(autondn.name)

#define _LOG_INFO(x) NDN_LOG_INFO(x)

#define _LOG_DEBUG(x) NDN_LOG_DEBUG(x)

#define _LOG_TRACE(x) NDN_LOG_TRACE(x)

#define _LOG_ERROR(x) NDN_LOG_ERROR(x)

#define _LOG_FATAL(x) NDN_LOG_FATAL(x)

} // autondn
} // ndn

#endif // AUTONDN_LOGGING_HPP
