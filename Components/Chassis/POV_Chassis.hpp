/*******************************************************************************
 * Copyright (c) 2024.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#ifndef FINEMOTE_POV_CHASSIS_H
#define FINEMOTE_POV_CHASSIS_H

#include <array>
#include "arm_math.h"
#include "Matrix/matrix.h"
#include "ChassisBase.hpp"

using Swerve_t = struct Swerve_t {
    MotorBase *steerMotor;
    MotorBase *driveMotor;
    float lx;
    float ly;
    float zeroPosition;
};

template<size_t N, typename OdomPolicy = WithoutOdom<3>>
class POV_Chassis : public ChassisBase<OdomPolicy> {
public:
    POV_Chassis(const float _wheelDiameter, std::array<Swerve_t, N> &&configs) : modules(std::move(configs)),
        wheelDiameter(_wheelDiameter) {
        for (int i = 0; i < N; ++i) {
            float hnData[2 * 3] = {1, 0, -modules[i].ly, 0, 1, modules[i].lx};
            Hn[i] = Matrixf<2, 3>(hnData);
            Jn[i] = matrixf::eye<3, 3>();
            Xn[i] = matrixf::zeros<3, 1>();
        }

        float BData[2 * 2] = {
            100, 0,
            0, 100
        };
        B = Matrixf<2, 2>(BData);
        float QData[3 * 3] = {
            0.1, 0, 0,
            0, 0.1, 0,
            0, 0, 0.1
        };
        Q = Matrixf<3, 3>(QData);
    }

    void InverseKinematics(std::array<float, 3> &v) final {
        // Inverse Kinematics
        for (auto &module: modules) {
            float vx = v[0] - module.ly * v[2];
            float vy = v[1] + module.lx * v[2];
            float v = sqrtf(vx * vx + vy * vy);
            float angle = atan2f(vy, vx) * 180 / PI;

            bool isNegative = false;
            while (angle + module.zeroPosition - module.steerMotor->GetMultiTurnPosition() > 100) {
                //100是换向施密特门
                angle -= 180;
                isNegative = !isNegative;
            }
            while (angle + module.zeroPosition - module.steerMotor->GetMultiTurnPosition() < -100) {
                angle += 180;
                isNegative = !isNegative;
            }
            if (isNegative) {
                v = -v;
            }

            module.steerMotor->SetTargetAngle(angle + module.zeroPosition);
            module.driveMotor->SetTargetSpeed(v / wheelDiameter / PI * 360);
        }
    }

    void ForwardKinematics() final {
        struct SwerveState {
            float angle{0};
            Matrixf<2, 1> vel;

            SwerveState() = default;

            SwerveState(const float _angle, const float _vel, const float &wheelDiameter, const float angleOffset) {
                angle = (_angle + angleOffset) / 180.f * PI; //弧度制
                float tmp[2]{
                    _vel / 360.f * PI * wheelDiameter * cosf(angle),
                    _vel / 360.f * PI * wheelDiameter * sinf(angle)
                };
                vel = tmp;
            }
        };

        std::array<Matrixf<3, 3>, N> Wn;
        std::array<Matrixf<3, 1>, N> wn;
        std::array<SwerveState, N> states;

        for (int i = 0; i < N; ++i) {
            SwerveState swerveState_t(modules[i].steerMotor->GetState().position,
                                      modules[i].driveMotor->GetState().speed, wheelDiameter, modules[i].zeroPosition);
            states[i] = swerveState_t;
            Xn[i] = alpha * Xn[i] + (1 - alpha) * Matrixf<3, 1>(this->targetV.data());
            Jn[i] = matrixf::inv(alpha * alpha * matrixf::inv(Jn[i]) + 0.5 * Q);
            Wn[i] = Jn[i] / N + Hn[i].trans() * B * Hn[i];
            wn[i] = Jn[i] / N * Xn[i] + Hn[i].trans() * B * states[i].vel;
        }

        Matrixf<3, 3> W_mean = matrixf::zeros<3, 3>();
        Matrixf<3, 1> w_mean = matrixf::zeros<3, 1>();
        for (auto W_t: Wn) W_mean += W_t / N;
        for (auto W_t: Wn) W_t = W_mean;
        for (auto w_t: wn) w_mean += w_t / N;
        for (auto w_t: wn) w_t = w_mean;

        for (int i = 0; i < N; ++i) {
            Xn[i] = matrixf::inv(W_mean) * w_mean;
            Jn[i] = N * W_mean;
        }
        this->estimatedV[0] = Xn[0][0][0];
        this->estimatedV[1] = Xn[0][1][0];
        this->estimatedV[2] = Xn[0][2][0];
    }

    void Handle() final {
        ForwardKinematics();
        if (!std::is_same<OdomPolicy, WithoutOdom<3>>::value) {
            this->odom.UpdateOdom(this->estimatedV, this->divisionFactor);
        }
        // Control
        InverseKinematics(this->targetV);
    }

private:
    std::array<Swerve_t, N> modules;
    std::array<Matrixf<3, 3>, N> Jn;
    std::array<Matrixf<2, 3>, N> Hn;
    std::array<Matrixf<3, 1>, N> Xn;
    Matrixf<3, 3> Q;
    Matrixf<2, 2> B;
    const float alpha = 0.5;
    const float wheelDiameter;
};

template<typename OdomPolicy = WithoutOdom<3>, typename... Configs>
auto POV_ChassisBuilder(float _wheelDiameter, Configs &&... configs) {
    constexpr size_t N = sizeof...(Configs);
    static_assert((std::is_same<std::decay_t<Configs>, Swerve_t>::value && ...),
                  "All configs must be of type Swerve_t");
    std::array<Swerve_t, N> modules = {std::forward<Configs>(configs)...};
    return POV_Chassis<N, OdomPolicy>(_wheelDiameter, std::move(modules));
}

#endif
