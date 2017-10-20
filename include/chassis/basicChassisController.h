#ifndef OKAPI_ROBOTCONTROLLER
#define OKAPI_ROBOTCONTROLLER

#include "chassis/chassisModel.h"
#include "control/pid.h"
#include "control/mpController.h"
#include "motionProfile/mpGenerator.h"
#include "odometry/odometry.h"
#include <API.h>
#include <memory>

namespace okapi {
  class ChassisController {
  public:
    ChassisController(const ChassisModelParams& imodelParams):
      model(imodelParams.make()) {}

    ChassisController(const std::shared_ptr<ChassisModel>& imodel):
      model(imodel) {}

    virtual ~ChassisController() = default;

    /**
     * Drives the robot straight
     * @param itarget Distance to travel
     */
    virtual void driveStraight(const int itarget) = 0;

    /**
     * Turns the robot clockwise in place
     * @param idegTarget Degrees to turn for
     */
    virtual void pointTurn(const float idegTarget) = 0;

    //Passed through to internal ChassisModel
    void driveForward(const int power) { model->driveForward(power); }
    void driveVector(const int distPower, const int anglePower) { model->driveVector(distPower, anglePower); }
    void turnClockwise(const int power) { model->turnClockwise(power); }
    void stop() { model->stop(); }
    void tank(const int leftVal, const int rightVal) { model->tank(leftVal, rightVal); }
    void arcade(const int verticalVal, const int horizontalVal) { model->arcade(verticalVal, horizontalVal); }
    void left(const int val) { model->left(val); }
    void right(const int val) { model->right(val); }
    std::valarray<int> getEncoderVals() const { return model->getEncoderVals(); }
  protected:
    std::shared_ptr<ChassisModel> model;
  };

  class ChassisControllerPid : public virtual ChassisController {
  public:
    ChassisControllerPid(const ChassisModelParams& imodelParams, const PidParams& idistanceParams, const PidParams& iangleParams):
      ChassisController(imodelParams),
      distancePid(idistanceParams),
      anglePid(iangleParams) {}

    ChassisControllerPid(const std::shared_ptr<ChassisModel>& imodel, const PidParams& idistanceParams, const PidParams& iangleParams):
      ChassisController(imodel),
      distancePid(idistanceParams),
      anglePid(iangleParams) {}

      virtual ~ChassisControllerPid() {
        delete &distancePid;
        delete &anglePid;
      }

      /**
       * Drives the robot straight
       * @param itarget Distance to travel
       */
      void driveStraight(const int itarget) override;

      /**
       * Turns the robot clockwise in place
       * @param idegTarget Degrees to turn for
       */
      void pointTurn(float idegTarget) override;
  protected:
    Pid distancePid, anglePid;
  };

  class ChassisControllerMP : public virtual ChassisController {
  public:
    ChassisControllerMP(const ChassisModelParams& imodelParams, const MPControllerParams& iparams):
      ChassisController(imodelParams),
      controller(iparams) {}


    ChassisControllerMP(const std::shared_ptr<ChassisModel>& imodel, const MPControllerParams& iparams):
      ChassisController(imodel),
      controller(iparams) {}

    virtual ~ChassisControllerMP() {
      delete &controller;
    }

    /**
     * Drives the robot straight
     * @param itarget Distance to travel
     */
    void driveStraight(const int itarget) override;

    /**
     * Turns the robot clockwise in place
     * @param idegTarget Degrees to turn for
     */
    void pointTurn(float idegTarget) override;
  protected:
    MPController controller;
  };
}

#endif /* end of include guard: OKAPI_ROBOTCONTROLLER */
