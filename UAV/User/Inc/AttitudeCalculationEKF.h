

#ifndef __ATTITUDECALCULATIONEKF_H
#define __ATTITUDECALCULATIONEKF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "arm_math.h"

class AttitudeCalculationEKF
{
	public:
		void Init(float ai[],float gi[],float mi[]);
	  void CalcAttitudeAGM_EKF(float ai[],float gi[],float mi[],float dt,float out[] = 0);
	  void CalcAttitudeAG_EKF(float ai[],float gi[],float dt,float out[] = 0);	
	  void GetOutAngle(float out[]);
	  void ConversionEfToBf(float ef[3],float bf[3]);
	  void GetAccelerationEf(float a[3]);
	  float GetVibrationFrequency(void);
	  float GetThrottleCompensation(void);
	  float X_copy[12];
	protected:
		void MatrixMultTranspose(arm_matrix_instance_f32 *m1,arm_matrix_instance_f32 *m2,arm_matrix_instance_f32*mOut);
		void NormalizationVector(float v[3]);
		void CrossVector(float v1[3],float v2[3],float vOut[3]);
    void CrossMatrix(float v[3],float m[3 * 3]);
    void PredictionX(float prex[12],float dt);
    void PredictionP(float A[12 * 12],float Pk_1[12 * 12],float Qk_1[12]);
    void CalculationA(float A[12 * 12],float Xk_1[12],float dt);
    void CalculationK(float K[12 * 9],float preP[12 * 12],float Rk_1[9]);
    void UpdataX(float preX[12],float Z[9],float K[12 * 9]);
    void UpdataP(float preP[12 * 12],float K[12 * 9]);
	  void CalculationRef(float X[12]);
  private:
		float xRef[3],zRef[3],yRef[3];
	  float subtractGA[3];
	  float vibrationFrequency;
};

#ifdef __cplusplus
}
#endif

#endif 



