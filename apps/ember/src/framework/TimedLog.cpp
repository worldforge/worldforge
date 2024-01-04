/*
 Copyright (C) 2009 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "TimedLog.h"

#include <utility>

#include "Log.h"

namespace Ember {
#if ENABLE_TIMED_LOG == 1

TimedLog::TimedLog(std::string logName, bool reportStart) :
		mLogName(std::move(logName)), mStart(std::chrono::steady_clock::now()) {
	if (reportStart) {
		logger->debug("Started task '{}'.", mLogName);
	}
}

TimedLog::~TimedLog() {
	auto currentTime = std::chrono::steady_clock::now();
	auto microDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - mStart);
	logger->debug("Ended task '{}' after {} microseconds.", mLogName, microDuration.count());
}

void TimedLog::report() {
	auto currentTime = std::chrono::steady_clock::now();
	auto microDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - mStart);
	if (mLastReport.time_since_epoch().count()) {
		auto microLastReportDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - mLastReport);
		logger->debug("Reported on task '{}' after {} microseconds, {} since last reported time.", mLogName, microDuration.count(), microLastReportDuration.count());
	} else {
		logger->debug("Reported on task '{}' after {} microseconds.", mLogName, microDuration.count());
	}
	mLastReport = currentTime;
}

void TimedLog::report(const std::string& reportName) {
	auto currentTime = std::chrono::steady_clock::now();
	auto microDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - mStart);
	if (mLastReport.time_since_epoch().count()) {
		auto microLastReportDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - mLastReport);
		logger->debug("Reported '{}' on task '{}' after {} microseconds, {} since last reported time.", reportName, mLogName, microDuration.count(), microLastReportDuration.count());
	} else {
		logger->debug("Reported '{}' on task '{}' after {} microseconds.", reportName, mLogName, microDuration.count());
	}
	mLastReport = currentTime;
}

#else
TimedLog::TimedLog(std::string logName, bool reportStart)
{
}

TimedLog::~TimedLog() = default;

void TimedLog::report()
{
}

void TimedLog::report(const std::string& reportName)
{
}
#endif
}
