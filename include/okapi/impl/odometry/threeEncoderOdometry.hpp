/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _OKAPI_THREEENCODERODOMETRY_HPP_
#define _OKAPI_THREEENCODERODOMETRY_HPP_

#include "okapi/api/chassis/model/threeEncoderSkidSteerModel.hpp"
#include "okapi/impl/odometry/odometry.hpp"

namespace okapi {
class ThreeEncoderOdometryArgs : public OdometryArgs {
  public:
  ThreeEncoderOdometryArgs(std::shared_ptr<SkidSteerModel> imodel, const double iscale,
                           const double iturnScale, const double imiddleScale);

  const double middleScale;
};

class ThreeEncoderOdometry : public Odometry {
  public:
  /**
   * Odometry. Tracks the movement of the robot and estimates its position in coordinates
   * relative to the start (assumed to be (0, 0)).
   *
   * @param imodelArgs ChassisModel for reading sensors
   * @param iscale straight scale
   * @param iturnScale turn scale
   * @param imiddleScale turn scale for the middle encoder (mounted perpendicular to the two side
   * encoders)
   */
  ThreeEncoderOdometry(std::shared_ptr<ThreeEncoderSkidSteerModel> imodel, const double iscale,
                       const double iturnScale, const double imiddleScale);

  /**
   * Do odometry math in an infinite loop.
   */
  virtual void loop() override;

  /**
   * Tread the input as a ThreeEncoderOdometry pointer and call loop. Meant to be used to bounce
   * into a thread because loop runs forever.
   *
   * @param context pointer to a ThreeEncoderOdometry object
   */
  static void trampoline(void *context);

  protected:
  std::shared_ptr<ThreeEncoderSkidSteerModel> model;
  const double middleScale;
  std::valarray<std::int32_t> lastTicks{0, 0, 0};
};
} // namespace okapi

#endif