/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "okapi/chassis/controller/chassisControllerPid.hpp"
#include "okapi/util/timer.hpp"
#include <cmath>

namespace okapi {
ChassisControllerPID::ChassisControllerPID(const ChassisModelParams &imodelParams,
                                           const PosPIDControllerParams &idistanceParams,
                                           const PosPIDControllerParams &iangleParams)
  : ChassisController(imodelParams), distancePid(idistanceParams), anglePid(iangleParams) {
}

ChassisControllerPID::ChassisControllerPID(std::shared_ptr<const ChassisModel> imodel,
                                           const PosPIDControllerParams &idistanceParams,
                                           const PosPIDControllerParams &iangleParams)
  : ChassisController(imodel), distancePid(idistanceParams), anglePid(iangleParams) {
}

ChassisControllerPID::ChassisControllerPID(const ChassisModelParams &imodelParams,
                                           const PosPIDController &idistance,
                                           const PosPIDController &iangle)
  : ChassisController(imodelParams), distancePid(idistance), anglePid(iangle) {
}

ChassisControllerPID::ChassisControllerPID(std::shared_ptr<const ChassisModel> imodel,
                                           const PosPIDController &idistance,
                                           const PosPIDController &iangle)
  : ChassisController(imodel), distancePid(idistance), anglePid(iangle) {
}

ChassisControllerPID::~ChassisControllerPID() = default;

void ChassisControllerPID::driveStraight(const int itarget) {
  const auto encStartVals = model->getSensorVals();
  float distanceElapsed = 0, angleChange = 0, lastDistance = 0;
  uint32_t prevWakeTime = millis();

  distancePid.reset();
  anglePid.reset();
  distancePid.setTarget(static_cast<float>(itarget));
  anglePid.setTarget(0);

  bool atTarget = false;
  const int atTargetDistance = 15;
  const int threshold = 2;

  Timer atTargetTimer;

  const int timeoutPeriod = 250;

  std::valarray<int> encVals{0, 0};
  float distOutput, angleOutput;

  while (!atTarget) {
    encVals = model->getSensorVals() - encStartVals;
    distanceElapsed = static_cast<float>((encVals[0] + encVals[1])) / 2.0;
    angleChange = static_cast<float>(encVals[1] - encVals[0]);

    distOutput = distancePid.step(distanceElapsed);
    angleOutput = anglePid.step(angleChange);
    model->driveVector(static_cast<int>(distOutput * 127), static_cast<int>(angleOutput * 127));

    if (abs(itarget - static_cast<int>(distanceElapsed)) <= atTargetDistance)
      atTargetTimer.placeHardMark();
    else if (abs(static_cast<int>(distanceElapsed) - static_cast<int>(lastDistance)) <= threshold)
      atTargetTimer.placeHardMark();
    else
      atTargetTimer.clearHardMark();

    lastDistance = distanceElapsed;

    if (atTargetTimer.getDtFromHardMark() >= timeoutPeriod)
      atTarget = true;

    task_delay_until(&prevWakeTime, 15);
  }

  model->stop();
}

void ChassisControllerPID::pointTurn(float idegTarget) {
  const auto encStartVals = model->getSensorVals();
  float angleChange = 0, lastAngle = 0;
  uint32_t prevWakeTime = millis();

  while (idegTarget > 180)
    idegTarget -= 360;
  while (idegTarget <= -180)
    idegTarget += 360;

  anglePid.reset();
  anglePid.setTarget(static_cast<float>(idegTarget));

  bool atTarget = false;
  const int atTargetAngle = 10;
  const int threshold = 2;

  Timer atTargetTimer;

  const int timeoutPeriod = 250;

  std::valarray<int> encVals{0, 0};

  while (!atTarget) {
    encVals = model->getSensorVals() - encStartVals;
    angleChange = static_cast<float>(encVals[1] - encVals[0]);

    model->turnClockwise(static_cast<int>(anglePid.step(angleChange) * 127));

    if (fabs(idegTarget - angleChange) <= atTargetAngle)
      atTargetTimer.placeHardMark();
    else if (fabs(angleChange - lastAngle) <= threshold)
      atTargetTimer.placeHardMark();
    else
      atTargetTimer.clearHardMark();

    lastAngle = angleChange;

    if (atTargetTimer.getDtFromHardMark() >= timeoutPeriod)
      atTarget = true;

    task_delay_until(&prevWakeTime, 15);
  }

  model->stop();
}
} // namespace okapi
