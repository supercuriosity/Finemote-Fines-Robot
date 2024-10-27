/*******************************************************************************
 * Copyright (c) 2023.
 * IWIN-FINS Lab, Shanghai Jiao Tong University, Shanghai, China.
 * All rights reserved.
 ******************************************************************************/

#include "Chassis.h"

void Chassis::ChassisSetVelocity(float _fbV, float _lrV, float _rtV) {
	ChassisStopFlag = false;
	FBVelocity = _fbV;
	LRVelocity = _lrV;
	RTVelocity = _rtV;
}

void Chassis::ChassisStop() {
	ChassisStopFlag = true;

	SFL.Stop();
	SFR.Stop();
	SBL.Stop();
	SBR.Stop();
	CFR.Stop();
	CFL.Stop();
	CBL.Stop();
	CBR.Stop();
}

void Chassis::LSOdometry() {
	struct WheelSet {
		WheelSet(const float _angle, const float _vel) {
			angle = _angle / 180.f * PI; //弧度制
			float tmp[2];
			tmp[1] = _vel / 360.f * PI * WHEEL_DIAMETER * cosf(angle); //单位m/s
			tmp[0] = _vel / 360.f * PI * WHEEL_DIAMETER * sinf(angle);
			vel = tmp;
		}

		float angle{0};
		Matrixf<2, 1> vel;
	};

	WheelSet FR(SFR.GetState().position, -CFR.GetState().speed); //右侧轮毂电机反转，取负
	WheelSet FL(SFL.GetState().position, CFL.GetState().speed);
	WheelSet BL(SBL.GetState().position, CBL.GetState().speed);
	WheelSet BR(SBR.GetState().position, -CBR.GetState().speed);

	chassisVel = invA * (H1.trans() * FR.vel + H2.trans() * FL.vel + H3.trans() *
		BL.vel + H4.trans() * BR.vel);
	float WCSVeldata[3 * 1] = {
		chassisVel[0][0] * cosf(chassisPos[0][2]) - chassisVel[1][0] * sinf(chassisPos[0][2]),
		chassisVel[0][0] * sinf(chassisPos[0][2]) + chassisVel[1][0] * cosf(chassisPos[0][2]),
		chassisVel[0][2]
	};
	WCSVelocity = Matrixf<3, 1>(WCSVeldata);
	chassisPos += WCSVelocity * 0.001;

	FRX = FR.vel[0][0], FRY = FR.vel[1][0];
	FLX = FL.vel[0][0], FLY = FL.vel[1][0];
	BLX = BL.vel[0][0], BLY = BL.vel[1][0];
	BRX = BR.vel[0][0], BRY = BR.vel[1][0];

	x = chassisPos[0][0];
	y = chassisPos[1][0];
	yaw = chassisPos[2][0];
}

void Chassis::ICFOdometry() {
	struct WheelSet {
		WheelSet(const float _angle, const float _vel) {
			angle = _angle / 180.f * PI; //弧度制
			float tmp[2];
			tmp[1] = _vel / 360.f * PI * WHEEL_DIAMETER * cosf(angle); //单位m/s
			tmp[0] = _vel / 360.f * PI * WHEEL_DIAMETER * sinf(angle);
			vel = tmp;
		}

		float angle{0};
		Matrixf<2, 1> vel;
	};

	WheelSet FR(SFR.GetState().position, -CFR.GetState().speed); //右侧轮毂电机反转，取负
	WheelSet FL(SFL.GetState().position, CFL.GetState().speed);
	WheelSet BL(SBL.GetState().position, CBL.GetState().speed);
	WheelSet BR(SBR.GetState().position, -CBR.GetState().speed);

	FRX = FR.vel[0][0], FRY = FR.vel[1][0];
	FLX = FL.vel[0][0], FLY = FL.vel[1][0];
	BLX = BL.vel[0][0], BLY = BL.vel[1][0];
	BRX = BR.vel[0][0], BRY = BR.vel[1][0];

	float plannedVel[3]{LRVelocity, FBVelocity, RTVelocity};
	x1 = 0.5 * x1 + 0.5 * Matrixf<3, 1>(plannedVel);
	x2 = 0.5 * x1 + 0.5 * Matrixf<3, 1>(plannedVel);
	x3 = 0.5 * x1 + 0.5 * Matrixf<3, 1>(plannedVel);
	x4 = 0.5 * x1 + 0.5 * Matrixf<3, 1>(plannedVel);

	J1 = matrixf::inv(0.25 * matrixf::inv(J1) + 0.5 * Q);
	J2 = matrixf::inv(0.25 * matrixf::inv(J2) + 0.5 * Q);
	J3 = matrixf::inv(0.25 * matrixf::inv(J3) + 0.5 * Q);
	J4 = matrixf::inv(0.25 * matrixf::inv(J4) + 0.5 * Q);

	Matrixf<3, 3> V1 = 0.25 * J1 + H1.trans() * B * H1;
	Matrixf<3, 1> v1 = 0.25 * J1 * x1 + H1.trans() * B * FR.vel;
	Matrixf<3, 3> V2 = 0.25 * J2 + H2.trans() * B * H2;
	Matrixf<3, 1> v2 = 0.25 * J2 * x2 + H2.trans() * B * FL.vel;
	Matrixf<3, 3> V3 = 0.25 * J3 + H3.trans() * B * H3;
	Matrixf<3, 1> v3 = 0.25 * J3 * x3 + H3.trans() * B * BL.vel;
	Matrixf<3, 3> V4 = 0.25 * J4 + H4.trans() * B * H4;
	Matrixf<3, 1> v4 = 0.25 * J4 * x4 + H4.trans() * B * BR.vel;

	Matrixf<3, 3> V1_t = (1 - 3 * epi) * V1 + epi * (V2 + V3 + V4);
	Matrixf<3, 1> v1_t = (1 - 3 * epi) * v1 + epi * (v2 + v3 + v4);
	Matrixf<3, 3> V2_t = (1 - 3 * epi) * V2 + epi * (V1 + V3 + V4);
	Matrixf<3, 1> v2_t = (1 - 3 * epi) * v2 + epi * (v1 + v3 + v4);
	Matrixf<3, 3> V3_t = (1 - 3 * epi) * V3 + epi * (V2 + V1 + V4);
	Matrixf<3, 1> v3_t = (1 - 3 * epi) * v3 + epi * (v2 + v1 + v4);
	Matrixf<3, 3> V4_t = (1 - 3 * epi) * V4 + epi * (V2 + V3 + V1);
	Matrixf<3, 1> v4_t = (1 - 3 * epi) * v4 + epi * (v2 + v3 + v1);

	V1 = V1_t, v1 = v1_t;
	V2 = V2_t, v2 = v2_t;
	V3 = V3_t, v3 = v3_t;
	V4 = V4_t, v4 = v4_t;

	x1 = matrixf::inv(V1) * v1;
	J1 = 4 * V1;
	x2 = matrixf::inv(V2) * v2;
	J2 = 4 * V2;
	x3 = matrixf::inv(V3) * v3;
	J3 = 4 * V3;
	x4 = matrixf::inv(V4) * v4;
	J4 = 4 * V4;


	float WCSVeldata[3 * 1] = {
		x1[0][0] * cosf(chassisPos[0][2]) - x1[1][0] * sinf(chassisPos[0][2]),
		x1[0][0] * sinf(chassisPos[0][2]) + x1[1][0] * cosf(chassisPos[0][2]),
		x1[0][2]
	};
	WCSVelocity = Matrixf<3, 1>(WCSVeldata);
	chassisPos += WCSVelocity * 0.001;

	x = chassisPos[0][0];
	y = chassisPos[1][0];
	yaw = chassisPos[2][0];
}


void Chassis::WheelsSpeedCalc(float fbVelocity, float lrVelocity, float rtVelocity) {
	float ClassisSpeed[4];
	static float lastAngle[4]{};

	const float A = lrVelocity - rtVelocity * LENGTH / 2;
	const float B = lrVelocity + rtVelocity * LENGTH / 2;
	const float C = fbVelocity - rtVelocity * WIDTH / 2;
	const float D = fbVelocity + rtVelocity * WIDTH / 2;

	//计算四个轮子角度，单位：度
	float RFRAngle = atan2f(A, D) * 180 / PI;
	float RFLAngle = atan2f(A, C) * 180 / PI;
	float RBLAngle = atan2f(B, C) * 180 / PI;
	float RBRAngle = atan2f(B, D) * 180 / PI;

	//计算四个轮子线速度，单位：度/s
	ClassisSpeed[0] = -sqrtf(A * A + D * D) / (WHEEL_DIAMETER * PI) * 360; //右前轮1
	ClassisSpeed[1] = sqrtf(A * A + C * C) / (WHEEL_DIAMETER * PI) * 360; //左前轮2
	ClassisSpeed[2] = sqrtf(B * B + C * C) / (WHEEL_DIAMETER * PI) * 360; //左后轮3
	ClassisSpeed[3] = -sqrtf(B * B + D * D) / (WHEEL_DIAMETER * PI) * 360; //右后轮4

	float motorAngleState[4];
	motorAngleState[0] = fmodf(SFR.GetState().position,360);
	motorAngleState[0] += motorAngleState[0]>180?-360:0;
	motorAngleState[1] = fmodf(SFL.GetState().position,360);
	motorAngleState[1] += motorAngleState[1]>180?-360:0;
	motorAngleState[2] = fmodf(SBL.GetState().position,360);
	motorAngleState[2] += motorAngleState[2]>180?-360:0;
	motorAngleState[3] = fmodf(SBR.GetState().position,360);
	motorAngleState[3] += motorAngleState[3]>180?-360:0;
	if(fabsf(SFR.GetState().position-RFRAngle) < 270 && fabsf(SFR.GetState().position-RFRAngle)>90){
		ClassisSpeed[0]*=-1;
		RFRAngle+=RFRAngle>0?-180:180;
	}
	if(fabsf(SFL.GetState().position-RFLAngle) < 270 && fabsf(SFL.GetState().position-RFLAngle)>90){
		ClassisSpeed[1]*=-1;
		RFLAngle+=RFLAngle>0?-180:180;
	}
	if(fabsf(SBL.GetState().position-RBLAngle) < 270 && fabsf(SBL.GetState().position-RBLAngle)>90){
		ClassisSpeed[2]*=-1;
		RBLAngle+=RBLAngle>0?-180:180;
	}
	if(fabsf(SBR.GetState().position-RBRAngle) < 270 && fabsf(SBR.GetState().position-RBRAngle)>90){
		ClassisSpeed[3]*=-1;
		RBRAngle+=RBRAngle>0?-180:180;
	}


	if(fabsf(fbVelocity)==0.0 && fabsf(lrVelocity)==0.0 && fabsf(rtVelocity)==0.0)
	{
		RFRAngle = lastAngle[0];
		RFLAngle = lastAngle[1];
		RBLAngle = lastAngle[2];
		RBRAngle = lastAngle[3];
	}

	//设置底盘电机角度
	SFR.SetTargetAngle(RFRAngle);
	SFL.SetTargetAngle(RFLAngle);
	SBL.SetTargetAngle(RBLAngle);
	SBR.SetTargetAngle(RBRAngle);

	//设置底盘电机转速
	CFR.SetTargetSpeed(ClassisSpeed[0]);
	CFL.SetTargetSpeed(ClassisSpeed[1]);
	CBL.SetTargetSpeed(ClassisSpeed[2]);
	CBR.SetTargetSpeed(ClassisSpeed[3]);

	lastAngle[0]=RFRAngle;
	lastAngle[1]=RFLAngle;
	lastAngle[2]=RBLAngle;
	lastAngle[3]=RBRAngle;
}


void Chassis::Handle() {
	WheelsSpeedCalc(FBVelocity, LRVelocity, RTVelocity);
	// LSOdometry();
	ICFOdometry();
}


ChassisBuilder Chassis::Build() {
	return {};
}

void Chassis::ResetOdometry(float _x = 0, float _y = 0, float _angle = 0) {
	// chassisPos = matrixf::zeros<3, 1>();
	chassisPos[0][0] = _x;
	x = _x;
	chassisPos[1][0] = _y;
	y = _y;
	chassisPos[2][0] = _angle;
	yaw = _angle;
}








