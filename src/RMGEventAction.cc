// Copyright (C) 2022 Luigi Pertoldi <gipert@pm.me>
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "RMGEventAction.hh"

#include <chrono>
#include <sstream>

#include "G4AnalysisManager.hh"
#include "G4GenericMessenger.hh"
#include "G4RunManager.hh"

#include "RMGHardware.hh"
#include "RMGLog.hh"
#include "RMGManager.hh"
#include "RMGRun.hh"
#include "RMGRunAction.hh"
#include "RMGUserAction.hh"

#include "fmt/chrono.h"
#include "magic_enum/magic_enum.hpp"

RMGEventAction::RMGEventAction(RMGRunAction* run_action) : fRunAction(run_action) {}

void RMGEventAction::BeginOfEventAction(const G4Event* event) {

  auto print_modulo = RMGManager::Instance()->GetPrintModulo();
  if ((event->GetEventID() + 1) % print_modulo == 0) {

    auto current_run = dynamic_cast<const RMGRun*>(G4RunManager::GetRunManager()->GetCurrentRun());
    auto tot_events = current_run->GetNumberOfEventToBeProcessed();

    auto start_time = current_run->GetStartTime();
    auto time_now = std::chrono::system_clock::now();
    auto tot_elapsed_s =
        std::chrono::duration_cast<std::chrono::seconds>(time_now - start_time).count();
    long partial = 0;
    long elapsed_d = (tot_elapsed_s - partial) / 86400;
    partial += elapsed_d * 86400;
    long elapsed_h = (tot_elapsed_s - partial) / 3600;
    partial += elapsed_h * 3600;
    long elapsed_m = (tot_elapsed_s - partial) / 60;
    partial += elapsed_m * 60;
    long elapsed_s = tot_elapsed_s - partial;

    // FIXME: how to cast the percentage to int?
    RMGLog::OutFormat(RMGLog::summary, "Processing event nr. {:d} ({:.0f}%), at {:d} days, {:d} hours, {:d} minutes and {} seconds",
        event->GetEventID() + 1, (event->GetEventID() + 1) * 100. / tot_events, elapsed_d,
        elapsed_h, elapsed_m, elapsed_s);
  }

  if (RMGManager::Instance()->IsPersistencyEnabled()) { fRunAction->ClearOutputDataFields(); }
}

void RMGEventAction::EndOfEventAction(const G4Event* event) {

  auto det_cons = RMGManager::Instance()->GetDetectorConstruction();
  auto active_dets = det_cons->GetActiveDetectorList();

  for (const auto& d_type : active_dets) {
    fRunAction->GetOutputDataFields(d_type)->EndOfEventAction(event);
  }

  // NOTE: G4analysisManager::AddNtupleRow() must be called here for event-oriented output
}

// vim: tabstop=2 shiftwidth=2 expandtab
