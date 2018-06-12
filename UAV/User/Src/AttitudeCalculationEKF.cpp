#include "AttitudeCalculationEKF.h"
#include <math.h>



//static float X1[12] = {1,0,0,0,0,0,0,0,-9.81f,200,0,0};
static float P[144];//,P1[144];
static float Q[12] = {1E-10f,1E-10f,1E-10f,1E-12,1E-12f,1E-12f,1E-10f,1E-10f,1E-10f,1E-10f,1E-10f,1E-10f};
static float R[9] = {1E-8f,1E-8f,1E-8f,1E-3f,1E-3f,1E-3f,1E-3f,1E-3f,1E-3f};

void AttitudeCalculationEKF::Init(float ai[],float gi[],float mi[])
{
  int i;
	float g[3] = {0,0,0};
	
	for(i = 0 ; i < 144 ; i ++)
	{
		P[i] = 1;
	}
	
	for(int i = 0 ; i < 3 ; i ++)
	{
		X_copy[i + 6] = ai[i];
		X_copy[i + 9] = mi[i];
	}
	
	for(i = 0 ; i < 5000 ; i ++)
	{
		CalcAttitudeAGM_EKF(ai,g,mi,0.002f,0);
	}
}

void AttitudeCalculationEKF::NormalizationVector(float v[3])
{
	float sum = 0;
	
	for(int i = 0 ; i < 3 ; i ++)
	{
		sum += v[i] * v[i];
	}
	sum = sqrtf(sum);
	
	if(sum == 0)
		return;
	for(int i = 0 ; i < 3 ; i ++)
	{
		v[i] /= sum;
	}
}


void AttitudeCalculationEKF::CrossVector(float v1[3],float v2[3],float vOut[3])
{
	vOut[0] = v1[1] * v2[2] - v1[2] * v2[1]; 
	vOut[1] = v1[2] * v2[0] - v1[0] * v2[2]; 
	vOut[2] = v1[0] * v2[1] - v1[1] * v2[0]; 
}

void AttitudeCalculationEKF::CrossMatrix(float v[3],float m[3 * 3])
{
	m[0] = m[4] = m[8] = 0;
	m[1] = -v[2];
	m[2] = v[1];
	m[3] = v[2];
	m[5] = -v[0];
	m[6] = -v[1];
	m[7] = v[0];
}


void AttitudeCalculationEKF::MatrixMultTranspose(arm_matrix_instance_f32 *m1,arm_matrix_instance_f32 *m2,arm_matrix_instance_f32*mOut)
{
	int i , j , k;
	float sum;
	for( i = 0 ; i < m1->numRows ; i ++)
	{
		for( j = 0 ; j < m2->numRows ; j ++)
		{
			sum = 0.0f;
			for( k = 0 ; k < m1->numCols ; k ++)
			{
				sum += m1->pData[i * m1->numRows + k] * m2->pData[j * m2->numRows + k];
			}
			mOut->pData[i * m2->numRows + j] = sum;
		}
	}
	mOut->numRows = m1->numRows;
	mOut->numCols = m2->numRows;
	return;
}

void AttitudeCalculationEKF::PredictionX(float prex[12],float dt)
{
	int i;
	float temp[3];
	
	CrossVector(prex,prex + 6,temp);
	for(i = 0 ; i < 3 ; i ++)
	{
		prex[6 + i] -= temp[i] * dt;
	}
	
	CrossVector(prex,prex + 9,temp);
	for(i = 0 ; i < 3 ; i ++)
	{
		prex[9 + i] -= temp[i] * dt;
	}
	
	for(i = 0 ; i < 3 ; i ++)
	{
		prex[i] += prex[i + 3] * dt;
	}
}

void AttitudeCalculationEKF::PredictionP(float A[12 * 12],float Pk_1[12 * 12],float Qk_1[12])
{
	int i;
	float Temp[144];
	
	arm_matrix_instance_f32 matrixA,matrixP,matrixTemp;
	matrixA.numCols = matrixA.numRows = matrixP.numCols = matrixP.numRows = matrixTemp.numCols = matrixTemp.numRows = 12;
	matrixA.pData = A;
	matrixP.pData = Pk_1;
	matrixTemp.pData = Temp;
	
	arm_mat_mult_f32(&matrixA,&matrixP,&matrixTemp);
	MatrixMultTranspose(&matrixTemp,&matrixA,&matrixP);
	
	for(i = 0 ; i < 12 ; i ++)
	{
		Pk_1[i * 12 + i] += Qk_1[i];
	}
}

void AttitudeCalculationEKF::CalculationA(float A[12 * 12],float Xk_1[12],float dt)
{
	int i,j;
	float m[9];
	
	for(i = 0 ; i < 12 ; i ++)
	{
		for(j = 0 ; j < 12 ; j ++)
		{
			A[i * 12  + j] = 0;
		}
		A[i * 12 + i ] = 1;
	}
	
	for(i = 0 ; i < 3 ; i ++)
	{
		A[12 * i + 3 + i] = dt;
	}
	
	CrossMatrix(Xk_1,m);
	for(i = 0 ; i < 3 ; i ++)
	{
		for(j = 0 ; j < 3 ; j ++)
		{
			A[(i + 6) * 12 + j + 6] -= m[i * 3 + j] * dt;
			A[(i + 9) * 12 + j + 9] -= m[i * 3 + j] * dt;
		}
	}
	
	CrossMatrix(Xk_1 + 6,m);
	
	for(i = 0 ; i < 3 ; i ++)
	{
		for(j = 0 ; j < 3 ; j ++)
		{
			A[(i + 6) * 12 + j] = m[i * 3 + j] * dt;
		}
	}
	
	CrossMatrix(Xk_1 + 9,m);
	
	for(i = 0 ; i < 3 ; i ++)
	{
		for(j = 0 ; j < 3 ; j ++)
		{
			A[(i + 9) * 12 + j] = m[i * 3 + j] * dt;
		}
	}
}

void AttitudeCalculationEKF::CalculationK(float K[12 * 9],float preP[12 * 12],float Rk_1[9])
{
	float Temp1[12 * 9];
	float Temp2[9 * 9];
	float Temp3[9 * 9];
	int i,j;
	arm_matrix_instance_f32 matrixK,matrixTemp2,matrixTemp3;
	matrixK.numRows = 12;
	matrixK.numCols = 9;
	
	matrixTemp2.numCols = matrixTemp2.numRows = matrixTemp3.numCols = matrixTemp3.numRows = 9;
	matrixK.pData = K;
	matrixTemp2.pData = Temp2;
	matrixTemp3.pData = Temp3;
	
	for(i = 0 ; i < 12 ; i ++)
	{
		for(j = 0 ; j < 3 ; j ++)
		{
			Temp1[i * 9 + j] = preP[i * 12 + j];
		}
		for(j = 3 ; j < 9 ; j ++)
		{
			Temp1[i * 9 + j] = preP[i * 12 + j + 3];
		}
	}
	
	for(i = 0 ; i < 3 ; i ++)
	{
		for(j = 0 ; j < 3 ; j ++)
		{
			Temp2[i * 9 + j] = preP[i * 12 + j];
		}
		for(j = 3 ; j < 9 ; j ++)
		{
			Temp2[i * 9 + j] = preP[i * 12 + j + 3];
		}
	}
	
	for(i = 3 ; i < 9 ; i ++)
	{
		for(j = 0 ; j < 3 ; j ++)
		{
			Temp2[i * 9 + j] = preP[(i + 3)* 12 + j];
		}
		for(j = 3 ; j < 9 ; j ++)
		{
			Temp2[i * 9 + j] = preP[(i + 3)* 12 + j + 3];
		}
	}
	
	for(i = 0 ; i < 9 ; i ++)
	{
		Temp2[i * 9 + i] += Rk_1[i];
	}
	
	arm_mat_inverse_f32(&matrixTemp2,&matrixTemp3) ;
	
	matrixTemp2.numRows = 12;
	matrixTemp2.numCols = 9;
	matrixTemp2.pData = Temp1;
	arm_mat_mult_f32(&matrixTemp2,&matrixTemp3,&matrixK);
}

void AttitudeCalculationEKF::UpdataX(float preX[12],float Z[9],float K[12 * 9])
{
	float Temp1[9],Temp2[12];
	int i;
	arm_matrix_instance_f32 matrixK,matrixTemp1,matrixTemp2;
	matrixTemp1.numRows = 9;
	matrixTemp1.numCols = 1;
	matrixTemp2.numRows = 12;
	matrixTemp2.numCols = 1;
	matrixK.numRows = 12;
	matrixK.numCols = 9;
	matrixK.pData = K;
	matrixTemp1.pData = Temp1;
	matrixTemp2.pData = Temp2;
	
	for(i = 0 ; i < 3 ; i ++)
	{
		Temp1[i] = Z[i] - preX[i];	
	}
	
	for(i = 3 ; i < 9 ; i ++)
	{
		Temp1[i] = Z[i] - preX[i + 3];	
	}
	arm_mat_mult_f32(&matrixK,&matrixTemp1,&matrixTemp2);
	
	for(i = 0 ; i < 12 ; i ++)
	{
		preX[i] += Temp2[i];
	}
}

void AttitudeCalculationEKF::UpdataP(float preP[12 * 12],float K[12 * 9])
{
	int i,j;
	float Temp1[12 * 12];
	float Temp2[12 * 12];
	
	arm_matrix_instance_f32 matrixP,matrixTemp1,matrixTemp2;
	matrixTemp1.numRows = 12;
	matrixTemp1.numCols = 12;
	matrixTemp2.numRows = 12;
	matrixTemp2.numCols = 12;
	matrixP.numCols = 12;
	matrixP.numRows = 12;
	matrixP.pData = preP;
	matrixTemp1.pData = Temp1;
	matrixTemp2.pData = Temp2;
	
	for(i = 0 ; i < 12 ; i ++)
	{
		for(j = 0 ; j < 3 ; j ++)
		{
			Temp1[i * 12 + j] = - K[ i * 9 + j];
		}
		for(j = 3 ; j < 6 ; j ++)
		{
			Temp1[i * 12 + j] = 0;
		}
		for(j = 6 ; j < 12 ; j ++)
		{
			Temp1[i * 12 + j] = - K[ i * 9 + j - 3];
		}
		Temp1[ i * 12 + i] += 1;
	}
	arm_mat_mult_f32(&matrixTemp1,&matrixP,&matrixTemp2);
	memcpy(preP,Temp2,sizeof(Temp2));
}

void AttitudeCalculationEKF::CalculationRef(float X[12])
{
  zRef[0] = - X[7];
	zRef[1] = - X[6];
	zRef[2] = X[8];
	
	xRef[0] = X[10];
	xRef[1] = X[9];
	xRef[2] = - X[11];
	
	CrossVector(zRef,xRef,yRef);
	CrossVector(yRef,zRef,xRef);
	NormalizationVector(xRef);
	NormalizationVector(yRef);
	NormalizationVector(zRef);
}

void AttitudeCalculationEKF::CalcAttitudeAGM_EKF(float ai[3],float gi[3],float mi[3],float dt,float out[6])
{
	float A[144];
	float Z[9];
	float K[12 * 9];
	float X[12];
	
	for(int i = 0 ; i < 3 ; i ++)
	{
		Z[i] = gi[i];
		Z[i + 3] = ai[i];
		Z[i + 6] = mi[i];
	}
	memcpy(X,X_copy,4 * 12);
	
	CalculationA(A,X,dt);
	PredictionP(A,P,Q);
	CalculationK(K,P,R);
	PredictionX(X,dt);
	UpdataX(X,Z,K);
	UpdataP(P,K);
  CalculationRef(X);
	
	this->vibrationFrequency = 0;
	for(int i = 0 ; i < 3 ; i ++)
	{
	  subtractGA[i] = ai[i] - X[i + 6];
		this->vibrationFrequency += subtractGA[i] * subtractGA[i];
	}
	
	memcpy(X_copy,X,4 * 12);
}

void AttitudeCalculationEKF::CalcAttitudeAG_EKF(float ai[],float gi[],float dt,float out[])
{
   
}

void AttitudeCalculationEKF::GetOutAngle(float out[])
{
	out[1] = - asinf(zRef[0]);
	out[0] = atan2f(zRef[1],zRef[2]);
	out[2] = atan2f(yRef[0],xRef[0]);
	
	out[3] = X_copy[1];
	out[4] = X_copy[0];
	out[5] = - X_copy[2];
}

void AttitudeCalculationEKF::ConversionEfToBf(float ef[3],float bf[3])
{
	float yRotating[3];
	float rate,*p1,*p2;

  if(abs(yRef[0]) > abs(xRef[0]))	
	{		
		p1 = xRef;
		p2 = yRef;
	}
	else
	{
		p1 = yRef;
		p2 = xRef;
	}
	
	rate = p1[0] / p2[0];
	yRotating[0] = 0;
	yRotating[1] = p1[1] - rate * p2[1];
	yRotating[2] = p1[2] - rate * p2[2];
	
	if(yRotating[1] < 0)
	{
	  yRotating[1] = - yRotating[1];
	  yRotating[2] = - yRotating[2];
	}
	
	NormalizationVector(yRotating);
	bf[0] = ef[0] + ef[2] * zRef[0];
	bf[1] = ef[1] * yRotating[1] + ef[2] * zRef[1];
	bf[2] = ef[1] * yRotating[2] + ef[2] * zRef[2];
}

void AttitudeCalculationEKF::GetAccelerationEf(float a[3])
{
	float temp[3];
	temp[0] = subtractGA[1];
	temp[1] = subtractGA[0];
	temp[2] = - subtractGA[2];
	
	a[0] = temp[0] * xRef[0] + temp[1] * xRef[1] + temp[2] * xRef[2];
	a[1] = temp[0] * yRef[0] + temp[1] * yRef[1] + temp[2] * yRef[2];
	a[2] = temp[0] * zRef[0] + temp[1] * zRef[1] + temp[2] * zRef[2];
}

float AttitudeCalculationEKF::GetVibrationFrequency(void)
{
	return this->vibrationFrequency;
}

float AttitudeCalculationEKF::GetThrottleCompensation(void)
{
	return zRef[0] * zRef[0] + zRef[1] * zRef[1];
}

