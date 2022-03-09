#ifndef _RMG_MANAGEMENT_RUN_ACTION_HH_
#define _RMG_MANAGEMENT_RUN_ACTION_HH_

#include <chrono>

#include "globals.hh"
#include "G4UserRunAction.hh"

class G4Run;
class RMGRun;
class RMGMasterGenerator;
class RMGRunAction : public G4UserRunAction {

  public:

    RMGRunAction(bool persistency=false);
    RMGRunAction(RMGMasterGenerator*, bool persistency=false);
    ~RMGRunAction() = default;

    RMGRunAction           (RMGRunAction const&) = delete;
    RMGRunAction& operator=(RMGRunAction const&) = delete;
    RMGRunAction           (RMGRunAction&&)      = delete;
    RMGRunAction& operator=(RMGRunAction&&)      = delete;

    G4Run* GenerateRun() override;
    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

  private:

    RMGRun* fRMGRun = nullptr;
    bool fIsPersistencyEnabled = false;
    RMGMasterGenerator* fRMGMasterGenerator = nullptr;
};

#endif

// vim: tabstop=2 shiftwidth=2 expandtab
