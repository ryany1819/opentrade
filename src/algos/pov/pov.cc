#include "../twap/twap.h"

namespace opentrade {

struct Pov : public TWAP {
  std::string OnStart(const ParamMap& params) noexcept override {
    auto err = TWAP::OnStart(params);
    if (!err.empty()) return err;
    if (max_pov_ <= 0) return "MaxPov required";
    window_ = GetParam(params, "Window", 0) * 60;
    if (window_ > 0) {
      roll_market_vol_.Initialize(window_, initial_volume_);
      roll_my_vol_.Initialize(window_, 0);
    }
    return {};
  }

  const ParamDefs& GetParamDefs() noexcept {
    static ParamDefs kDefs =
        CombineParamDefs(kCommonParamDefs, ParamDefs{
                                               {"Window", 0, false},
                                           });
    return kDefs;
  }

  double GetLeaves() noexcept override {
    double exposure, pov;
    if (window_ > 0) {
      pov = roll_market_vol_.GetValue();
      exposure = roll_my_vol_.GetValue() + inst_->total_outstanding_qty();
    } else {
      pov = md().trade.volume - initial_volume_;
      exposure = inst_->total_exposure(true);
    }
    pov *= max_pov_;
    return pov + (min_size_ > 0 ? min_size_ : inst_->sec().lot_size) - exposure;
  }

  void Timer() noexcept override {
    if (window_ > 0) {
      auto time = GetTime();
      roll_market_vol_.Update(md().trade.volume, time);
      roll_my_vol_.Update(inst_->cum_qty(true), time);
    }
    TWAP::Timer();
  }

  void OnStop() noexcept override {
    TWAP::OnStop();
    roll_market_vol_.Clear();
    roll_my_vol_.Clear();
  }

 private:
  int window_ = 0;
  RollDelta<MarketData::Volume> roll_market_vol_{0, 0};
  RollDelta<MarketData::Volume> roll_my_vol_{0, 0};
};

}  // namespace opentrade

extern "C" {
opentrade::Adapter* create() { return new opentrade::Pov{}; }
}
