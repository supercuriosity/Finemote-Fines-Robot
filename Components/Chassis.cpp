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

void Chassis::Odometry() {
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
		chassisVel[0][0] * cosf(chassisPos[0][2]) + chassisVel[1][0] * sinf(chassisPos[0][2]),
		-chassisVel[0][0] * sinf(chassisPos[0][2]) + chassisVel[1][0] * cosf(chassisPos[0][2]),
		chassisVel[0][2]
	};
	WCSVelocity = Matrixf<3, 1>(WCSVeldata);
	chassisPos += WCSVelocity * 0.001;

	FRX=FR.vel[0][0],FRY=FR.vel[1][0];
	FLX=FL.vel[0][0],FLY=FL.vel[1][0];
	BLX=BL.vel[0][0],BLY=BL.vel[1][0];
	BRX=BR.vel[0][0],BRY=BR.vel[1][0];

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

	FRX=FR.vel[0][0],FRY=FR.vel[1][0];
	FLX=FL.vel[0][0],FLY=FL.vel[1][0];
	BLX=BL.vel[0][0],BLY=BL.vel[1][0];
	BRX=BR.vel[0][0],BRY=BR.vel[1][0];

	Matrixf<3,3> V1=0.25*J1+H1.trans()*B*H1;
	Matrixf<3,1> v1=0.25*J1*x1+H1.trans()*B*FR.vel;
	Matrixf<3,3> V2=0.25*J2+H2.trans()*B*H2;
	Matrixf<3,1> v2=0.25*J2*x2+H2.trans()*B*FL.vel;
	Matrixf<3,3> V3=0.25*J3+H3.trans()*B*H3;
	Matrixf<3,1> v3=0.25*J3*x3+H3.trans()*B*BL.vel;
	Matrixf<3,3> V4=0.25*J4+H4.trans()*B*H4;
	Matrixf<3,1> v4=0.25*J4*x4+H4.trans()*B*BR.vel;

	Matrixf<3,3> V1_t=(1-3*epi)*V1+epi*(V2+V3+V4);
	Matrixf<3,1> v1_t=(1-3*epi)*v1+epi*(v2+v3+v4);
	Matrixf<3,3> V2_t=(1-3*epi)*V2+epi*(V1+V3+V4);
	Matrixf<3,1> v2_t=(1-3*epi)*v2+epi*(v1+v3+v4);
	Matrixf<3,3> V3_t=(1-3*epi)*V3+epi*(V2+V1+V4);
	Matrixf<3,1> v3_t=(1-3*epi)*v3+epi*(v2+v1+v4);
	Matrixf<3,3> V4_t=(1-3*epi)*V4+epi*(V2+V3+V1);
	Matrixf<3,1> v4_t=(1-3*epi)*v4+epi*(v2+v3+v1);

	V1=V1_t,v1=v1_t;
	V2=V2_t,v2=v2_t;
	V3=V3_t,v3=v3_t;
	V4=V4_t,v4=v4_t;

	x1=matrixf::inv(V1)*v1;
	J1=4*V1;
	x2=matrixf::inv(V2)*v2;
	J2=4*V2;
	x3=matrixf::inv(V3)*v3;
	J3=4*V3;
	x4=matrixf::inv(V4)*v4;
	J4=4*V4;

	J1=matrixf::inv(matrixf::inv(J1)+Q);
	J2=matrixf::inv(matrixf::inv(J2)+Q);
	J3=matrixf::inv(matrixf::inv(J3)+Q);
	J4=matrixf::inv(matrixf::inv(J4)+Q);

	float WCSVeldata[3 * 1] = {
		x1[0][0] * cosf(chassisPos[0][2]) + x1[1][0] * sinf(chassisPos[0][2]),
		-x1[0][0] * sinf(chassisPos[0][2]) + x1[1][0] * cosf(chassisPos[0][2]),
		x1[0][2]
	};
	WCSVelocity = Matrixf<3, 1>(WCSVeldata);
	chassisPos += WCSVelocity * 0.001;
}


void Chassis::WheelsSpeedCalc(float fbVelocity, float lrVelocity,float rtVelocity) {
	float ClassisSpeed[4];
	float RFLAngle, RFRAngle, RBLAngle, RBRAngle;
	float vx, vy, w;

	vx = lrVelocity;
	vy = fbVelocity;
	w = rtVelocity;
	float A, B, C, D;
	A = vx + w * LENGTH / 2;
	B = vx - w * LENGTH / 2;
	C = vy + w * WIDTH / 2;
	D = vy - w * WIDTH / 2;

	//计算四个轮子角度，单位：度
	RFRAngle = atan2f(A, D) * 180 / PI;
	RFLAngle = atan2f(A, C) * 180 / PI;
	RBLAngle = atan2f(B, C) * 180 / PI;
	RBRAngle = atan2f(B, D) * 180 / PI;

	//计算四个轮子线速度，单位：度/s
	ClassisSpeed[0] = -sqrtf(A * A + D * D) / (WHEEL_DIAMETER * PI) * 360; //右前轮1
	ClassisSpeed[1] = sqrtf(A * A + C * C) / (WHEEL_DIAMETER * PI) * 360; //左前轮2
	ClassisSpeed[2] = sqrtf(B * B + C * C) / (WHEEL_DIAMETER * PI) * 360; //左后轮3
	ClassisSpeed[3] = -sqrtf(B * B + D * D) / (WHEEL_DIAMETER * PI) * 360; //右后轮4

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
}


void Chassis::Handle() {
	WheelsSpeedCalc(FBVelocity, LRVelocity, RTVelocity);
	// Odometry();
	ICFOdometry();
}


ChassisBuilder Chassis::Build() {
	return {};
}

void Chassis::ResetOdometry(float _x=0,float _y=0,float _angle=0) {
	// chassisPos = matrixf::zeros<3, 1>();
	chassisPos[0][0] = _x;
	x=_x;
	chassisPos[1][0] = _y;
	y=_y;
	chassisPos[2][0] = _angle;
	yaw=_angle;
}








