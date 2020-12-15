#include "RMGManager.hh"

#include "G4RunManager.hh"
#include "G4VisManager.hh"

#include "RMGLog.hh"
#include "RMGManagerMessenger.hh"

RMGManager* RMGManager::fRMGManager = nullptr;

RMGManager::RMGManager() {
  if (fRMGManager) RMGLog::Out(RMGLog::fatal, "RMGManager must be singleton!");
  fRMGManager = this;
  fG4Messenger = new RMGManagerMessenger(this);
}

RMGManager::~RMGManager() {
  delete fG4Messenger;
  if (RMGLog::IsOpen()) RMGLog::CloseLog();
}

// vim: tabstop=2 shiftwidth=2 expandtab
