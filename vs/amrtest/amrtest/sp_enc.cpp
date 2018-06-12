/*
 * ===================================================================
 *  TS 26.104
 *  REL-5 V5.4.0 2004-03
 *  REL-6 V6.1.0 2004-03
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

/*
 * sp_enc.c
 *
 *
 * Project:
 *    AMR Floating-Point Codec
 *
 * Contains:
 *    This module contains all the functions needed encoding 160
 *    16-bit speech samples to AMR encoder parameters.
 *
 */
#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <memory.h>
#include <math.h>
#include <float.h>
#include "sp_enc.h"
#include "rom_enc.h"


#define DebugPrintf(format,...)   

/*
 * Definition of structures used in encoding process
 */
typedef struct
{
   Float32 y2;
   Float32 y1;
   Float32 x0;
   Float32 x1;

}Pre_ProcessState;

#ifdef VAD2

/* Defines for VAD2 */
#define	FRM_LEN1		80
#define	DELAY0			24
#define	FFT_LEN1		128

#define	UPDATE_CNT_THLD1	50

#define	INIT_FRAMES		4

#define	CNE_SM_FAC1		0.1
#define	CEE_SM_FAC1		0.55

#define	HYSTER_CNT_THLD1	6	/* forced update constants... */
#define	HIGH_ALPHA1		0.9
#define	LOW_ALPHA1		0.7
#define	ALPHA_RANGE1		(HIGH_ALPHA1-LOW_ALPHA1)

#define NORM_ENRG		(4.0)	/* account for div by 2 by the HPF */
#define	MIN_CHAN_ENRG		(0.0625 / NORM_ENRG)
#define	INE			(16.0 / NORM_ENRG)
#define	NOISE_FLOOR		(1.0 / NORM_ENRG)

#define	PRE_EMP_FAC1		(-0.8)

#define	NUM_CHAN		16
#define	LO_CHAN			0
#define	HI_CHAN			15
#define	UPDATE_THLD		35

#define	SINE_START_CHAN		2
#define	P2A_THRESH		10.0
#define	DEV_THLD1		28.0

/* Defines for the FFT function */
#define	SIZE			128
#define	SIZE_BY_TWO		64
#define	NUM_STAGE		6

#define	PI			3.141592653589793

#define	TRUE			1
#define	FALSE			0

/* Macros */
#define	min(a,b)		((a)<(b)?(a):(b))
#define	max(a,b)		((a)>(b)?(a):(b))
#define	square(a)		((a)*(a))

/* structures */
typedef struct
{
  Float32 pre_emp_mem;
  Word16  update_cnt;
  Word16  hyster_cnt;
  Word16  last_update_cnt;
  Float32 ch_enrg_long_db[NUM_CHAN];
  Word32  Lframe_cnt;
  Float32 ch_enrg[NUM_CHAN];
  Float32 ch_noise[NUM_CHAN];
  Float32 tsnr;
  Word16  hangover;
  Word16  burstcount;
  Word16  fupdate_flag;
  Float32 negSNRvar;
  Float32 negSNRbias;
  Float32 R0;
  Float32 Rmax;
  Word16  LTP_flag;
}vadState;
#else
typedef struct
{
   Float32 bckr_est[COMPLEN];   /* background noise estimate */
   Float32 ave_level[COMPLEN];


   /* averaged input components for stationary estimation */
   Float32 old_level[COMPLEN];   /* input levels of the previous frame */
   Float32 sub_level[COMPLEN];


   /* input levels calculated at the end of a frame (lookahead) */
   Float32 a_data5[3][2];   /* memory for the filter bank */
   Float32 a_data3[5];   /* memory for the filter bank */
   Float32 best_corr_hp;   /* FIP filtered value */


   /* counts length of a speech burst incl HO addition */
   Float32 corr_hp_fast;   /* filtered value */
   Word32 vadreg;   /* flags for intermediate VAD decisions */
   Word32 pitch;   /* flags for pitch detection */
   Word32 oldlag_count, oldlag;   /* variables for pitch detection */
   Word32 complex_high;   /* flags for complex detection */
   Word32 complex_low;   /* flags for complex detection */
   Word32 complex_warning;   /* complex background warning */
   Word32 tone;   /* flags for tone detection */
   Word16 burst_count;   /* counts length of a speech burst */
   Word16 hang_count;   /* hangover counter */
   Word16 stat_count;   /* stationary counter */
   Word16 complex_hang_count;   /* complex hangover counter, used by VAD */
   Word16 complex_hang_timer;   /* hangover initiator, used by CAD */
   Word16 speech_vad_decision;   /* final decision */
   Word16 sp_burst_count;


}vadState;
#endif
#define DTX_HIST_SIZE 8
#define DTX_ELAPSED_FRAMES_THRESH (24 + 7 -1)
#define DTX_HANG_CONST 7   /* yields eight frames of SP HANGOVER */
typedef struct
{
   Float32 lsp_hist[M * DTX_HIST_SIZE];
   Float32 log_en_hist[DTX_HIST_SIZE];
   Word32 init_lsf_vq_index;
   Word16 hist_ptr;
   Word16 log_en_index;
   Word16 lsp_index[3];


   /* DTX handler stuff */
   Word16 dtxHangoverCount;
   Word16 decAnaElapsedCount;


}dtx_encState;
typedef struct
{
   /* gain history */
   Float32 gp[N_FRAME];


   /* counters */
   Word16 count;


}tonStabState;
typedef struct
{
   Word32 past_qua_en[4];


   /* normal MA predictor memory, (contains 20*log10(qua_err)) */
}gc_predState;

typedef struct
{
   Float32 prev_alpha;   /* previous adaptor output, */
   Float32 prev_gc;   /* previous code gain, */
   Float32 ltpg_mem[LTPG_MEM_SIZE];   /* LTP coding gain history, */
   Word16 onset;   /* onset state, */


   /* (ltpg_mem[0] not used for history) */
}gain_adaptState;
typedef struct
{

   Float32 sf0_target_en;
   Float32 sf0_coeff[5];
   Word32 sf0_gcode0_exp;
   Word32 sf0_gcode0_fra;
   Word16 *gain_idx_ptr;


   gc_predState * gc_predSt;
   gc_predState * gc_predUncSt;
   gain_adaptState * adaptSt;
}gainQuantState;
typedef struct
{
   Word32 T0_prev_subframe;   /* integer pitch lag of previous sub-frame */


}Pitch_frState;
typedef struct
{
   Pitch_frState * pitchSt;
}clLtpState;
typedef struct
{
   Float32 ada_w;
   Word32 old_T0_med;
   Word16 wght_flg;


}pitchOLWghtState;
typedef struct
{
   Float32 past_rq[M];   /* Past quantized prediction error */


}Q_plsfState;
typedef struct
{
   /* Past LSPs */
   Float32 lsp_old[M];
   Float32 lsp_old_q[M];


   /* Quantization state */
   Q_plsfState * qSt;
}lspState;
typedef struct
{
   Float32 old_A[M + 1];   /* Last A(z) for case of unstable filter */


}LevinsonState;
typedef struct
{
   LevinsonState * LevinsonSt;
}lpcState;
typedef struct
{
   /* Speech vector */
   Float32 old_speech[L_TOTAL];
   Float32 *speech, *p_window, *p_window_12k2;
   Float32 *new_speech;   /* Global variable */


   /* Weight speech vector */
   Float32 old_wsp[L_FRAME + PIT_MAX];
   Float32 *wsp;


   /* OL LTP states */
   Word32 old_lags[5];
   Float32 ol_gain_flg[2];


   /* Excitation vector */
   Float32 old_exc[L_FRAME + PIT_MAX + L_INTERPOL];
   Float32 *exc;


   /* Zero vector */
   Float32 ai_zero[L_SUBFR + MP1];
   Float32 *zero;


   /* Impulse response vector */
   Float32 *h1;
   Float32 hvec[L_SUBFR * 2];


   /* Substates */
   lpcState * lpcSt;
   lspState * lspSt;
   clLtpState * clLtpSt;
   gainQuantState * gainQuantSt;
   pitchOLWghtState * pitchOLWghtSt;
   tonStabState * tonStabSt;
   vadState * vadSt;

   Word32 dtx;


   dtx_encState * dtxEncSt;

   /* Filter's memory */
   Float32 mem_syn[M], mem_w0[M], mem_w[M];
   Float32 mem_err[M + L_SUBFR], *error;
   Float32 sharp;


}cod_amrState;
typedef struct
{
   cod_amrState * cod_amr_state;
   Pre_ProcessState * pre_state;

   Word32 dtx;


}Speech_Encode_FrameState;


/*
 * Dotproduct40
 *
 *
 * Parameters:
 *    x                 I: First input
 *    y                 I: Second input
 * Function:
 *    Computes dot product size 40
 *
 * Returns:
 *    acc                dot product
 */
static Float64 Dotproduct40( Float32 *x, Float32 *y )
{
   Float64 acc;


   acc = x[0] * y[0] + x[1] * y[1] + x[2] * y[2] + x[3] * y[3];
   acc += x[4] * y[4] + x[5] * y[5] + x[6] * y[6] + x[7] * y[7];
   acc += x[8] * y[8] + x[9] * y[9] + x[10] * y[10] + x[11] * y[11];
   acc += x[12] * y[12] + x[13] * y[13] + x[14] * y[14] + x[15] * y[15];
   acc += x[16] * y[16] + x[17] * y[17] + x[18] * y[18] + x[19] * y[19];
   acc += x[20] * y[20] + x[21] * y[21] + x[22] * y[22] + x[23] * y[23];
   acc += x[24] * y[24] + x[25] * y[25] + x[26] * y[26] + x[27] * y[27];
   acc += x[28] * y[28] + x[29] * y[29] + x[30] * y[30] + x[31] * y[31];
   acc += x[32] * y[32] + x[33] * y[33] + x[34] * y[34] + x[35] * y[35];
   acc += x[36] * y[36] + x[37] * y[37] + x[38] * y[38] + x[39] * y[39];
   return( acc );
}


/*
 * Autocorr
 *
 *
 * Parameters:
 *    x                 I: Input signal
 *    r                 O: Autocorrelations
 *    wind              I: Window for LPC analysis
 * Function:
 *    Calculate autocorrelation with window, LPC order = M
 *
 * Returns:
 *    void
 */
static void Autocorr( Float32 x[], Float32 r[], const Float32 wind[] )
{
   Word32 i, j;   /* Counters */
   Float32 y[L_WINDOW + M + 1];   /* Windowed signal */
   Float64 sum;   /* temp */


   /*
    * Windowing of signal
    */
   for ( i = 0; i < L_WINDOW; i++ ) {
      y[i] = x[i] * wind[i];
   }

   /*
    * Zero remaining memory
    */
   memset( &y[L_WINDOW], 0, 44 );

   /*
    * Autocorrelation
    */
   for ( i = 0; i <= M; i++ ) {
      sum = 0;

      for ( j = 0; j < L_WINDOW; j += 40 ) {
         sum += Dotproduct40( &y[j], &y[j + i] );
      }
      r[i] = (Float32)sum;
   }
}


/*
 * Levinson
 *
 *
 * Parameters:
 *    old_A             I: Vector of old LP coefficients [M+1]
 *    r                 I: Vector of autocorrelations    [M+1]
 *    a                 O: LP coefficients               [M+1]
 *    rc                O: Reflection coefficients       [4]
 * Function:
 *    Levinson-Durbin algorithm
 *
 * Returns:
 *    void
 *
 */
static void Levinson( Float32 *old_A, Float32 *r, Float32 *A, Float32 *rc )
{
   Float32 sum, at, err;
   Word32 l, j, i;
   Float32 rct[M];   /* temporary reflection coefficients  0,...,m-1 */


   rct[0] = ( -r[1] ) / r[0];
   A[0] = 1.0F;
   A[1] = rct[0];
   err = r[0] + r[1] * rct[0];

   if ( err <= 0.0 )
      err = 0.01F;

   for ( i = 2; i <= M; i++ ) {
      sum = 0.0F;

      for ( j = 0; j < i; j++ )
         sum += r[i - j] * A[j];
      rct[i - 1] = ( -sum ) / ( err );

      for ( j = 1; j <= ( i / 2 ); j++ ) {
         l = i - j;
         at = A[j] + rct[i - 1] *A[l];
         A[l] += rct[i - 1] *A[j];
         A[j] = at;
      }
      A[i] = rct[i - 1];
      err += rct[i - 1] *sum;

      if ( err <= 0.0 )
         err = 0.01F;
   }
   memcpy( rc, rct, 4 * sizeof( Float32 ) );
   memcpy( old_A, A, MP1 * sizeof( Float32 ) );
}


/*
 * lpc
 *
 *
 * Parameters:
 *    old_A             O: Vector of old LP coefficients [M+1]
 *    x                 I: Input signal
 *    x_12k2            I: Input signal 12.2k
 *    a                 O: predictor coefficients
 *    mode              I: AMR mode
 * Function:
 *    LP analysis
 *
 *    In 12.2 kbit/s mode linear prediction (LP) analysis is performed
 *    twice per speech frame using the auto-correlation approach with
 *    30 ms asymmetric windows. No lookahead is used in
 *    the auto-correlation computation.
 *
 *    In other modes analysis is performed once per speech frame
 *    using the auto-correlation approach with 30 ms asymmetric windows.
 *    A lookahead of 40 samples (5 ms) is used in the auto-correlation computation.
 *
 *    The auto-correlations of windowed speech are converted to the LP
 *    coefficients using the Levinson-Durbin algorithm.
 *    Then the LP coefficients are transformed to the Line Spectral Pair
 *    (LSP) domain  for quantization and interpolation purposes.
 *    The interpolated quantified and unquantized filter coefficients
 *    are converted back to the LP filter coefficients
 *    (to construct the synthesis and weighting filters at each subframe).
 *
 * Returns:
 *    void
 *
 */
static void lpc( Float32 *old_A, Float32 x[], Float32 x_12k2[], Float32 a[], enum Mode
      mode )
{
   Word32 i;
   Float32 r[MP1];
   Float32 rc[4];


   if ( mode == MR122 ) {
      Autocorr( x_12k2, r, window_160_80 );

      /*
       * Lag windowing
       */
      for ( i = 1; i <= M; i++ ) {
         r[i] = r[i] * lag_wind[i - 1];
      }
      r[0] *= 1.0001F;

      if ( r[0] < 1.0F )
         r[0] = 1.0F;

      /*
       * Levinson Durbin
       */
      Levinson( old_A, r, &a[MP1], rc );

      /*
       * Autocorrelations
       */
      Autocorr( x_12k2, r, window_232_8 );

      /*
       * Lag windowing
       */
      for ( i = 1; i <= M; i++ ) {
         r[i] = r[i] * lag_wind[i - 1];
      }
      r[0] *= 1.0001F;

      if ( r[0] < 1.0F )
         r[0] = 1.0F;

      /*
       * Levinson Durbin
       */
      Levinson( old_A, r, &a[MP1 * 3], rc );
   }
  
}


/*
 * Chebps
 *
 *
 * Parameters:
 *    x                 I: Cosine value for the freqency given
 *    f                 I: angular freqency
 * Function:
 *    Evaluates the Chebyshev polynomial series
 *
 * Returns:
 *    result of polynomial evaluation
 */
static Float32 Chebps( Float32 x, Float32 f[] )
{
   Float32 b0, b1, b2, x2;
   Word32 i;


   x2 = 2.0F * x;
   b2 = 1.0F;
   b1 = x2 + f[1];

   for ( i = 2; i < 5; i++ ) {
      b0 = x2 * b1 - b2 + f[i];
      b2 = b1;
      b1 = b0;
   }
   return( x * b1 - b2 + f[i] );
}


/*
 * Az_lsp
 *
 *
 * Parameters:
 *    a                 I: Predictor coefficients              [MP1]
 *    lsp               O: Line spectral pairs                 [M]
 *    old_lsp           I: Old lsp, in case not found 10 roots [M]
 *
 * Function:
 *    LP to LSP conversion
 *
 *    The LP filter coefficients A[], are converted to the line spectral pair
 *    (LSP) representation for quantization and interpolation purposes.
 *
 * Returns:
 *    void
 */
static void Az_lsp( Float32 a[], Float32 lsp[], Float32 old_lsp[] )
{
   Word32 i, j, nf, ip;
   Float32 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
   Float32 y;
   Float32 *coef;
   Float32 f1[6], f2[6];


   /*
    *  find the sum and diff. pol. F1(z) and F2(z)
    */
   f1[0] = 1.0F;
   f2[0] = 1.0F;

   for ( i = 0; i < ( NC ); i++ ) {
      f1[i + 1] = a[i + 1] +a[M - i] - f1[i];
      f2[i + 1] = a[i + 1] -a[M - i] + f2[i];
   }
   f1[NC] *= 0.5F;
   f2[NC] *= 0.5F;

   /*
    * find the LSPs using the Chebychev pol. evaluation
    */
   nf = 0;   /* number of found frequencies */
   ip = 0;   /* indicator for f1 or f2 */
   coef = f1;
   xlow = grid[0];
   ylow = Chebps( xlow, coef );
   j = 0;

   while ( ( nf < M ) && ( j < 60 ) ) {
      j++;
      xhigh = xlow;
      yhigh = ylow;
      xlow = grid[j];
      ylow = Chebps( xlow, coef );

      if ( ylow * yhigh <= 0 ) {
         /* divide 4 times the interval */
         for ( i = 0; i < 4; i++ ) {
            xmid = ( xlow + xhigh ) * 0.5F;
            ymid = Chebps( xmid, coef );

            if ( ylow * ymid <= 0.0F ) {
               yhigh = ymid;
               xhigh = xmid;
            }
            else {
               ylow = ymid;
               xlow = xmid;
            }
         }

         /*
          * Linear interpolation
          * xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow)
          */
         y = yhigh - ylow;

         if ( y == 0 ) {
            xint = xlow;
         }
         else {
            y = ( xhigh - xlow ) / ( yhigh - ylow );
            xint = xlow - ylow * y;
         }
         lsp[nf] = xint;
         xlow = xint;
         nf++;

         if ( ip == 0 ) {
            ip = 1;
            coef = f2;
         }
         else {
            ip = 0;
            coef = f1;
         }
         ylow = Chebps( xlow, coef );
      }
   }

   /* Check if M roots found */
   if ( nf < M ) {
      memcpy( lsp, old_lsp, M <<2 );
   }
   return;
}


/*
 * Get_lsp_pol
 *
 *
 * Parameters:
 *    lsp                 I: line spectral frequencies
 *    f                   O: polynomial F1(z) or F2(z)
 *
 * Function:
 *    Find the polynomial F1(z) or F2(z) from the LSPs.
 *
 *    F1(z) = product ( 1 - 2 lsp[i] z^-1 + z^-2 )
 *             i=0,2,4,6,8
 *    F2(z) = product   ( 1 - 2 lsp[i] z^-1 + z^-2 )
 *             i=1,3,5,7,9
 *
 *    where lsp[] is the LSP vector in the cosine domain.
 *
 *    The expansion is performed using the following recursion:
 *
 *    f[0] = 1
 *    b = -2.0 * lsp[0]
 *    f[1] = b
 *    for i=2 to 5 do
 *       b = -2.0 * lsp[2*i-2];
 *       f[i] = b*f[i-1] + 2.0*f[i-2];
 *       for j=i-1 down to 2 do
 *          f[j] = f[j] + b*f[j-1] + f[j-2];
 *       f[1] = f[1] + b;
 *
 * Returns:
 *    void
 */
static void Get_lsp_pol( Float32 *lsp, Float32 *f )
{
   Word32 i, j;
   Float32 T0;


   f[0] = 1.0F;
   f[1] = -2.0F * lsp[0];

   for ( i = 2; i <= 5; i++ ) {
      T0 = -2.0F * lsp[2 * i - 2];
      f[i] = ( Float32 )( T0 * f[i - 1] +2.0F * f[i - 2] );

      for ( j = i - 1; j >= 2; j-- ) {
         f[j] = f[j] + T0 * f[j - 1] +f[j - 2];
      }
      f[1] = f[1] + T0;
   }
   return;
}


/*
 * Lsp_Az
 *
 *
 * Parameters:
 *    lsp                 I: Line spectral frequencies
 *    a                   O: Predictor coefficients
 *
 * Function:
 *    Converts from the line spectral pairs (LSP) to LP coefficients,
 *    for a 10th order filter.
 *
 * Returns:
 *    void
 */
static void Lsp_Az( Float32 lsp[], Float32 a[] )
{
   Float32 f1[6], f2[6];
   Word32 i, j;


   Get_lsp_pol( &lsp[0], f1 );
   Get_lsp_pol( &lsp[1], f2 );

   for ( i = 5; i > 0; i-- ) {
      f1[i] += f1[i - 1];
      f2[i] -= f2[i - 1];
   }
   a[0] = 1;

   for ( i = 1, j = 10; i <= 5; i++, j-- ) {
      a[i] = ( Float32 )( ( f1[i] + f2[i] ) * 0.5F );
      a[j] = ( Float32 )( ( f1[i] - f2[i] ) * 0.5F );
   }
   return;
}


/*
 * Int_lpc_1and3_2
 *
 *
 * Parameters:
 *    lsp_old        I: LSP vector at the 4th subfr. of past frame      [M]
 *    lsp_mid        I: LSP vector at the 2nd subframe of present frame [M]
 *    lsp_new        I: LSP vector at the 4th subframe of present frame [M]
 *    az             O: interpolated LP parameters in subframes 1 and 3
 *                                                                [AZ_SIZE]
 *
 * Function:
 *    Interpolation of the LPC parameters. Same as the Int_lpc
 *    function but we do not recompute Az() for subframe 2 and
 *	   4 because it is already available.
 *
 * Returns:
 *    void
 */
static void Int_lpc_1and3_2( Float32 lsp_old[], Float32 lsp_mid[], Float32
      lsp_new[], Float32 az[] )
{
   Float32 lsp[M];
   Word32 i;


   for ( i = 0; i < M; i += 2 ) {
      lsp[i] = ( lsp_mid[i] + lsp_old[i] ) * 0.5F;
      lsp[i + 1] = ( lsp_mid[i + 1] +lsp_old[i+1] )*0.5F;
   }

   /* Subframe 1 */
   Lsp_Az( lsp, az );
   az += MP1 * 2;

   for ( i = 0; i < M; i += 2 ) {
      lsp[i] = ( lsp_mid[i] + lsp_new[i] ) * 0.5F;
      lsp[i + 1] = ( lsp_mid[i + 1] +lsp_new[i+1] )*0.5F;
   }

   /* Subframe 3 */
   Lsp_Az( lsp, az );
   return;
}


/*
 * Lsp_lsf
 *
 *
 * Parameters:
 *    lsp               I: LSP vector
 *    lsf               O: LSF vector
 *
 * Function:
 *    Transformation lsp to lsf, LPC order M
 *
 * Returns:
 *    void
 */
static void Lsp_lsf( Float32 lsp[], Float32 lsf[] )
{
   Word32 i;


   for ( i = 0; i < M; i++ ) {
      lsf[i] = ( Float32 )( acos( lsp[i] )*SCALE_LSP_FREQ );
   }
   return;
}


/*
 * Lsf_wt
 *
 *
 * Parameters:
 *    lsf               I: LSF vector
 *    wf                O: square of weighting factors
 *
 * Function:
 *    Compute LSF weighting factors
 *
 * Returns:
 *    void
 */
static void Lsf_wt( Float32 *lsf, Float32 *wf )
{
   Float32 temp;
   Word32 i;


   wf[0] = lsf[1];

   for ( i = 1; i < 9; i++ ) {
      wf[i] = lsf[i + 1] -lsf[i - 1];
   }
   wf[9] = 4000.0F - lsf[8];

   for ( i = 0; i < 10; i++ ) {
      if ( wf[i] < 450.0F ) {
         temp = 3.347F - SLOPE1_WGHT_LSF * wf[i];
      }
      else {
         temp = 1.8F - SLOPE2_WGHT_LSF * ( wf[i] - 450.0F );
      }
      wf[i] = temp * temp;
   }
   return;
}


/*
 * Vq_subvec
 *
 *
 * Parameters:
 *    lsf_r1            I: 1st LSF residual vector
 *    lsf_r2            I: 2nd LSF residual vector
 *    dico              I: quantization codebook
 *    wf1               I: 1st LSF weighting factors
 *    wf2               I: 2nd LSF weighting factors
 *    dico_size         I: size of quantization codebook
 * Function:
 *    Quantization of a 4 dimensional subvector
 *
 * Returns:
 *    index             quantization index
 */
static Word16 Vq_subvec( Float32 *lsf_r1, Float32 *lsf_r2, const Float32 *dico,
      Float32 *wf1, Float32 *wf2, Word16 dico_size )
{
   Float64 temp, dist, dist_min;
   const Float32 *p_dico;
   Word32 i, index = 0;


   dist_min = DBL_MAX;
   p_dico = dico;

   for ( i = 0; i < dico_size; i++ ) {
      temp = lsf_r1[0] - *p_dico++;
      dist = temp * temp * wf1[0];
      temp = lsf_r1[1] - *p_dico++;
      dist += temp * temp * wf1[1];
      temp = lsf_r2[0] - *p_dico++;
      dist += temp * temp * wf2[0];
      temp = lsf_r2[1] - *p_dico++;
      dist += temp * temp * wf2[1];

      if ( dist < dist_min ) {
         dist_min = dist;
         index = i;
      }
   }

   /* Reading the selected vector */
   p_dico = &dico[index << 2];
   lsf_r1[0] = *p_dico++;
   lsf_r1[1] = *p_dico++;
   lsf_r2[0] = *p_dico++;
   lsf_r2[1] = *p_dico++;
   return( Word16 )index;
}


/*
 * Vq_subvec_s
 *
 *
 * Parameters:
 *    lsf_r1            I: 1st LSF residual vector
 *    lsf_r2            I: 2nd LSF residual vector
 *    dico              I: quantization codebook
 *    wf1               I: 1st LSF weighting factors
 *    wf2               I: 2nd LSF weighting factors
 *    dico_size         I: size of quantization codebook
 * Function:
 *    Quantization of a 4 dimensional subvector with a signed codebook
 *
 * Returns:
 *    index             quantization index
 */
static Word16 Vq_subvec_s( Float32 *lsf_r1, Float32 *lsf_r2, const Float32 *dico
      , Float32 *wf1, Float32 *wf2, Word16 dico_size )
{
   Float64 dist_min, dist1, dist2, temp1, temp2;
   const Float32 *p_dico;
   Word32 i, index = 0;
   Word16 sign = 0;


   dist_min = DBL_MAX;
   p_dico = dico;

   for ( i = 0; i < dico_size; i++ ) {
      temp1 = lsf_r1[0] - *p_dico;
      temp2 = lsf_r1[0] + *p_dico++;
      dist1 = temp1 * temp1 * wf1[0];
      dist2 = temp2 * temp2 * wf1[0];
      temp1 = lsf_r1[1] - *p_dico;
      temp2 = lsf_r1[1] + *p_dico++;
      dist1 += temp1 * temp1 * wf1[1];
      dist2 += temp2 * temp2 * wf1[1];
      temp1 = lsf_r2[0] - *p_dico;
      temp2 = lsf_r2[0] + *p_dico++;
      dist1 += temp1 * temp1 * wf2[0];
      dist2 += temp2 * temp2 * wf2[0];
      temp1 = lsf_r2[1] - *p_dico;
      temp2 = lsf_r2[1] + *p_dico++;
      dist1 += temp1 * temp1 * wf2[1];
      dist2 += temp2 * temp2 * wf2[1];

      if ( dist1 < dist_min ) {
         dist_min = dist1;
         index = i;
         sign = 0;
      }

      if ( dist2 < dist_min ) {
         dist_min = dist2;
         index = i;
         sign = 1;
      }
   }

   /* Reading the selected vector */
   p_dico = &dico[index << 2];

   if ( sign == 0 ) {
      lsf_r1[0] = *p_dico++;
      lsf_r1[1] = *p_dico++;
      lsf_r2[0] = *p_dico++;
      lsf_r2[1] = *p_dico++;
   }
   else {
      lsf_r1[0] = -( *p_dico++ );
      lsf_r1[1] = -( *p_dico++ );
      lsf_r2[0] = -( *p_dico++ );
      lsf_r2[1] = -( *p_dico++ );
   }
   index = index << 1;
   index = index + sign;
   return( Word16 )index;
}


/*
 * Reorder_lsf
 *
 *
 * Parameters:
 *    lsf               B: vector of LSFs
 *    min_dist          I: minimum required distance
 *
 * Function:
 *    Make sure that the LSFs are properly ordered and to keep a certain minimum
 *    distance between adjacent LSFs. LPC order = M.
 *
 * Returns:
 *    void
 */
static void Reorder_lsf( Float32 *lsf, Float32 min_dist )
{
   Float32 lsf_min;
   Word32 i;


   lsf_min = min_dist;

   for ( i = 0; i < M; i++ ) {
      if ( lsf[i] < lsf_min ) {
         lsf[i] = lsf_min;
      }
      lsf_min = lsf[i] + min_dist;
   }
}


/*
 * Lsf_lsp
 *
 *
 * Parameters:
 *    lsf               I: vector of LSFs
 *    lsp	            O: vector of LSPs
 *
 * Function:
 *    Transformation lsf to lsp, order M
 *
 * Returns:
 *    void
 */
static void Lsf_lsp( Float32 lsf[], Float32 lsp[] )
{
   Word32 i;


   for ( i = 0; i < M; i++ ) {
      lsp[i] = ( Float32 )cos( SCALE_FREQ_LSP * lsf[i] );
   }
   return;
}



/*
 * Q_plsf_5
 *
 *
 * Parameters:
 *    past_rq           B: past quantized residual
 *    lsp1              I: 1st LSP vector
 *    lsp2              I: 2nd LSP vector
 *    lsp1_q            O: quantized 1st LSP vector
 *    lsp2_q            O: quantized 2nd LSP vector
 *    indice	         I: quantization indices of 5 matrices and
 *                         one sign for 3rd
 *
 * Function:
 *    Quantization of 2 sets of LSF parameters using 1st order MA
 *    prediction and split by 5 matrix quantization (split-MQ).
 *
 * Returns:
 *    void
 */
static void Q_plsf_5( Float32 *past_rq, Float32 *lsp1, Float32 *lsp2, Float32 *
      lsp1_q, Float32 *lsp2_q, Word16 *indice )
{
   Float32 lsf1[M], lsf2[M], wf1[M], wf2[M], lsf_p[M], lsf_r1[M], lsf_r2[M];
   Float32 lsf1_q[M], lsf2_q[M];
   Word32 i;


   /* convert LSFs to normalize frequency domain */
   Lsp_lsf( lsp1, lsf1 );
   Lsp_lsf( lsp2, lsf2 );

   /* Compute LSF weighting factors */
   Lsf_wt( lsf1, wf1 );
   Lsf_wt( lsf2, wf2 );

   /* Compute predicted LSF and prediction error */
   for ( i = 0; i < M; i++ ) {
      /* MR122 LSP prediction factor = 0.65 */
      lsf_p[i] = mean_lsf_5[i] + past_rq[i] * 0.65F;
      lsf_r1[i] = lsf1[i] - lsf_p[i];
      lsf_r2[i] = lsf2[i] - lsf_p[i];
   }

   /* Split-MQ of prediction error */
   indice[0] = Vq_subvec( &lsf_r1[0], &lsf_r2[0], dico1_lsf_5, &wf1[0], &wf2[0],
         DICO1_SIZE_5 );
   indice[1] = Vq_subvec( &lsf_r1[2], &lsf_r2[2], dico2_lsf_5, &wf1[2], &wf2[2],
         DICO2_SIZE_5 );
   indice[2] = Vq_subvec_s( &lsf_r1[4], &lsf_r2[4], dico3_lsf_5, &wf1[4], &wf2[4
         ], DICO3_SIZE_5 );
   indice[3] = Vq_subvec( &lsf_r1[6], &lsf_r2[6], dico4_lsf_5, &wf1[6], &wf2[6],
         DICO4_SIZE_5 );
   indice[4] = Vq_subvec( &lsf_r1[8], &lsf_r2[8], dico5_lsf_5, &wf1[8], &wf2[8],
         DICO5_SIZE_5 );

   /* Compute quantized LSFs and update the past quantized residual */
   for ( i = 0; i < M; i++ ) {
      lsf1_q[i] = lsf_r1[i] + lsf_p[i];
      lsf2_q[i] = lsf_r2[i] + lsf_p[i];
      past_rq[i] = lsf_r2[i];
   }

   /* verification that LSFs has minimum distance of LSF_GAP 50hz */
   Reorder_lsf( lsf1_q, 50.0F );
   Reorder_lsf( lsf2_q, 50.0F );

   /*  convert LSFs to the cosine domain */
   Lsf_lsp( lsf1_q, lsp1_q );
   Lsf_lsp( lsf2_q, lsp2_q );
}


/*
 * Int_lpc_1and3
 *
 *
 * Parameters:
 *    lsp_old        I: LSP vector at the 4th subfr. of past frame      [M]
 *    lsp_mid        I: LSP vector at the 2nd subframe of present frame [M]
 *    lsp_new        I: LSP vector at the 4th subframe of present frame [M]
 *    az             O: interpolated LP parameters in subframes 1 and 3
 *                                                                [AZ_SIZE]
 *
 * Function:
 *    Interpolates the LSPs and converts to LPC parameters
 *    to get a different LP filter in each subframe.
 *
 *    The 20 ms speech frame is divided into 4 subframes.
 *    The LSPs are quantized and transmitted at the 2nd and
 *    4th subframes (twice per frame) and interpolated at the
 *    1st and 3rd subframe.
 *
 * Returns:
 *    void
 */
static void Int_lpc_1and3( Float32 lsp_old[], Float32 lsp_mid[], Float32 lsp_new
      [], Float32 az[] )
{
   Word32 i;
   Float32 lsp[M];


   for ( i = 0; i < M; i++ ) {
      lsp[i] = ( lsp_mid[i] + lsp_old[i] ) * 0.5F;
   }

   /* Subframe 1 */
   Lsp_Az( lsp, az );
   az += MP1;

   /* Subframe 2 */
   Lsp_Az( lsp_mid, az );
   az += MP1;

   for ( i = 0; i < M; i++ ) {
      lsp[i] = ( lsp_mid[i] + lsp_new[i] ) * 0.5F;
   }

   /* Subframe 3 */
   Lsp_Az( lsp, az );
   az += MP1;

   /* Subframe 4 */
   Lsp_Az( lsp_new, az );
   return;
}

/*
 * lsp
 *
 *
 * Parameters:
 *    req_mode          I: requested mode
 *    used_mode         I: used mode
 *    lsp_old           B: old LSP vector
 *    lsp_old_q         B: old quantized LSP vector
 *    past_rq           B: past quantized residual
 *    az                B: interpolated LP parameters
 *    azQ               O: quantization interpol. LP parameters
 *    lsp_new           O: new lsp vector
 *    anap              O: analysis parameters
 *
 * Function:
 *    From A(z) to lsp. LSP quantization and interpolation
 *
 * Returns:
 *    void
 */
static void lsp( enum Mode req_mode, enum Mode used_mode, Float32 *lsp_old,
      Float32 *lsp_old_q, Float32 *past_rq, Float32 az[], Float32 azQ[], Float32
      lsp_new[], Word16 **anap )
{
   Float32 lsp_new_q[M];   /* LSPs at 4th subframe */
   Float32 lsp_mid[M], lsp_mid_q[M];   /* LSPs at 2nd subframe */


   if ( req_mode == MR122 ) {
      Az_lsp( &az[MP1], lsp_mid, lsp_old );
      Az_lsp( &az[MP1 * 3], lsp_new, lsp_mid );

      /*
       * Find interpolated LPC parameters in all subframes
       * (both quantized and unquantized).
       * The interpolated parameters are in array A_t[] of size (M+1)*4
       * and the quantized interpolated parameters are in array Aq_t[]
       */
      Int_lpc_1and3_2( lsp_old, lsp_mid, lsp_new, az );

      if ( used_mode != MRDTX ) {
         /* LSP quantization (lsp_mid[] and lsp_new[] jointly quantized) */
         Q_plsf_5( past_rq, lsp_mid, lsp_new, lsp_mid_q, lsp_new_q, *anap );
         Int_lpc_1and3( lsp_old_q, lsp_mid_q, lsp_new_q, azQ );

         /* Advance analysis parameters pointer */
         ( *anap ) += 5;
      }
   }

   /* update the LSPs for the next frame */
   memcpy( lsp_old, lsp_new, M <<2 );
   memcpy( lsp_old_q, lsp_new_q, M <<2 );
}


/*
 * check_lsp
 *
 *
 * Parameters:
 *    count          B: counter for resonance
 *    lsp            B: LSP vector
 *
 * Function:
 *    Check the LSP's to detect resonances
 *
 *    Resonances in the LPC filter are monitored to detect possible problem
 *    areas where divergence between the adaptive codebook memories in
 *    the encoder and the decoder could cause unstable filters in areas
 *    with highly correlated continuos signals. Typically, this divergence
 *    is due to channel errors.
 *    The monitoring of resonance signals is performed using unquantized LSPs
 *    q(i), i = 1,...,10. The algorithm utilises the fact that LSPs are
 *    closely located at a peak in the spectrum. First, two distances,
 *    dist 1 and dist 2 ,are calculated in two different regions,
 *    defined as
 *
 *    dist1 = min[q(i) - q(i + 1)],  i = 4,...,8
 *    dist2 = min[q(i) - q(i + 1)],  i = 2,3
 *
 *    Either of these two minimum distance conditions must be fulfilled
 *    to classify the frame as a resonance frame and increase the resonance
 *    counter.
 *
 *    if(dist1 < TH1) || if (dist2 < TH2)
 *       counter++
 *    else
 *       counter = 0
 *
 *    TH1 = 0.046
 *    TH2 = 0.018, q(2) > 0.98
 *    TH2 = 0.024, 0.93 < q(2) <= 0.98
 *    TH2 = 0.018, otherwise
 *
 *    12 consecutive resonance frames are needed to indicate possible
 *    problem conditions, otherwise the LSP_flag is cleared.
 *
 * Returns:
 *    resonance flag
 */
static Word16 check_lsp( Word16 *count, Float32 *lsp )
{
   Float32 dist, dist_min1, dist_min2, dist_th;
   Word32 i;


   /*
    * Check for a resonance:
    * Find minimum distance between lsp[i] and lsp[i+1]
    */
   dist_min1 = FLT_MAX;

   for ( i = 3; i < 8; i++ ) {
      dist = lsp[i] - lsp[i + 1];

      if ( dist < dist_min1 ) {
         dist_min1 = dist;
      }
   }
   dist_min2 = FLT_MAX;

   for ( i = 1; i < 3; i++ ) {
      dist = lsp[i] - lsp[i + 1];

      if ( dist < dist_min2 ) {
         dist_min2 = dist;
      }
   }

   if ( lsp[1] > 0.98F ) {
      dist_th = 0.018F;
   }
   else if ( lsp[1] > 0.93F ) {
      dist_th = 0.024F;
   }
   else {
      dist_th = 0.034F;
   }

   if ( ( dist_min1 < 0.046F ) || ( dist_min2 < dist_th ) ) {
      *count += 1;
   }
   else {
      *count = 0;
   }

   /* Need 12 consecutive frames to set the flag */
   if ( *count >= 12 ) {
      *count = 12;
      return 1;
   }
   else {
      return 0;
   }
}


/*
 * Weight_Ai
 *
 *
 * Parameters:
 *    a                 I: LPC coefficients                    [M+1]
 *    fac               I: Spectral expansion factors.         [M+1]
 *    a_exp             O: Spectral expanded LPC coefficients  [M+1]
 *
 * Function:
 *    Spectral expansion of LP coefficients
 *
 * Returns:
 *    void
 */
static void Weight_Ai( Float32 a[], const Float32 fac[], Float32 a_exp[] )
{
   Word32 i;


   a_exp[0] = a[0];

   for ( i = 1; i <= M; i++ ) {
      a_exp[i] = a[i] * fac[i - 1];
   }
   return;
}


/*
 * Residu
 *
 *
 * Parameters:
 *    a                 I: prediction coefficients
 *    x                 I: speech signal
 *    y                 O: residual signal
 *
 * Function:
 *    Computes the LTP residual signal.
 *
 * Returns:
 *    void
 */
static void Residu( Float32 a[], Float32 x[], Float32 y[] )
{
   Float32 s;
   Word32 i;


   for ( i = 0; i < L_SUBFR; i += 4 ) {
      s = x[i] * a[0];
      s += x[i - 1] *a[1];
      s += x[i - 2] * a[2];
      s += x[i - 3] * a[3];
      s += x[i - 4] * a[4];
      s += x[i - 5] * a[5];
      s += x[i - 6] * a[6];
      s += x[i - 7] * a[7];
      s += x[i - 8] * a[8];
      s += x[i - 9] * a[9];
      s += x[i - 10] * a[10];
      y[i] = s;
      s = x[i + 1] *a[0];
      s += x[i] * a[1];
      s += x[i - 1] *a[2];
      s += x[i - 2] * a[3];
      s += x[i - 3] * a[4];
      s += x[i - 4] * a[5];
      s += x[i - 5] * a[6];
      s += x[i - 6] * a[7];
      s += x[i - 7] * a[8];
      s += x[i - 8] * a[9];
      s += x[i - 9] * a[10];
      y[i + 1] = s;
      s = x[i + 2] * a[0];
      s += x[i + 1] *a[1];
      s += x[i] * a[2];
      s += x[i - 1] *a[3];
      s += x[i - 2] * a[4];
      s += x[i - 3] * a[5];
      s += x[i - 4] * a[6];
      s += x[i - 5] * a[7];
      s += x[i - 6] * a[8];
      s += x[i - 7] * a[9];
      s += x[i - 8] * a[10];
      y[i + 2] = s;
      s = x[i + 3] * a[0];
      s += x[i + 2] * a[1];
      s += x[i + 1] *a[2];
      s += x[i] * a[3];
      s += x[i - 1] *a[4];
      s += x[i - 2] * a[5];
      s += x[i - 3] * a[6];
      s += x[i - 4] * a[7];
      s += x[i - 5] * a[8];
      s += x[i - 6] * a[9];
      s += x[i - 7] * a[10];
      y[i + 3] = s;
   }
   return;
}


/*
 * Syn_filt
 *
 *
 * Parameters:
 *    a                 I: prediction coefficients [M+1]
 *    x                 I: input signal
 *    y                 O: output signal
 *    mem               B: memory associated with this filtering
 *    update            I: 0=no update, 1=update of memory.
 *
 * Function:
 *    Perform synthesis filtering through 1/A(z).
 *
 * Returns:
 *    void
 */
static void Syn_filt( Float32 a[], Float32 x[], Float32 y[], Float32 mem[],
      Word16 update )
{
   Float64 tmp[50];
   Float64 sum;
   Float64 *yy;
   Word32 i;


   /* Copy mem[] to yy[] */
   yy = tmp;

   for ( i = 0; i < M; i++ ) {
      *yy++ = mem[i];
   }

   /* Do the filtering. */
   for ( i = 0; i < L_SUBFR; i = i + 4 ) {
      sum = x[i] * a[0];
      sum -= a[1] * yy[ - 1];
      sum -= a[2] * yy[ - 2];
      sum -= a[3] * yy[ - 3];
      sum -= a[4] * yy[ - 4];
      sum -= a[5] * yy[ - 5];
      sum -= a[6] * yy[ - 6];
      sum -= a[7] * yy[ - 7];
      sum -= a[8] * yy[ - 8];
      sum -= a[9] * yy[ - 9];
      sum -= a[10] * yy[ - 10];
      *yy++ = sum;
      y[i] = ( Float32 )yy[ - 1];
      sum = x[i + 1] *a[0];
      sum -= a[1] * yy[ - 1];
      sum -= a[2] * yy[ - 2];
      sum -= a[3] * yy[ - 3];
      sum -= a[4] * yy[ - 4];
      sum -= a[5] * yy[ - 5];
      sum -= a[6] * yy[ - 6];
      sum -= a[7] * yy[ - 7];
      sum -= a[8] * yy[ - 8];
      sum -= a[9] * yy[ - 9];
      sum -= a[10] * yy[ - 10];
      *yy++ = sum;
      y[i + 1] = ( Float32 )yy[ - 1];
      sum = x[i + 2] * a[0];
      sum -= a[1] * yy[ - 1];
      sum -= a[2] * yy[ - 2];
      sum -= a[3] * yy[ - 3];
      sum -= a[4] * yy[ - 4];
      sum -= a[5] * yy[ - 5];
      sum -= a[6] * yy[ - 6];
      sum -= a[7] * yy[ - 7];
      sum -= a[8] * yy[ - 8];
      sum -= a[9] * yy[ - 9];
      sum -= a[10] * yy[ - 10];
      *yy++ = sum;
      y[i + 2] = ( Float32 )yy[ - 1];
      sum = x[i + 3] * a[0];
      sum -= a[1] * yy[ - 1];
      sum -= a[2] * yy[ - 2];
      sum -= a[3] * yy[ - 3];
      sum -= a[4] * yy[ - 4];
      sum -= a[5] * yy[ - 5];
      sum -= a[6] * yy[ - 6];
      sum -= a[7] * yy[ - 7];
      sum -= a[8] * yy[ - 8];
      sum -= a[9] * yy[ - 9];
      sum -= a[10] * yy[ - 10];
      *yy++ = sum;
      y[i + 3] = ( Float32 )yy[ - 1];
   }

   /* Update of memory if update==1 */
   if ( update != 0 ) {
      for ( i = 0; i < M; i++ ) {
         mem[i] = y[30 + i];
      }
   }
   return;
}


/*
 * pre_big
 *
 *
 * Parameters:
 *    mode              I: AMR mode
 *    gamma1            I: spectral exp. factor 1
 *    gamma1_12k2       I: spectral exp. factor 1 for modes above MR795
 *    gamma2            I: spectral exp. factor 2
 *    A_t               I: A(z) unquantized, for 4 subframes
 *    frame_offset      I: frameoffset, 1st or second big_sbf
 *    speech            I: speech
 *    mem_w             B: synthesis filter memory state
 *    wsp               O: weighted speech
 *
 * Function:
 *    Big subframe (2 subframes) preprocessing
 *
 *    Open-loop pitch analysis is performed in order to simplify the pitch
 *    analysis and confine the closed-loop pitch search to a small number of
 *    lags around the open-loop estimated lags.
 *    Open-loop pitch estimation is based on the weighted speech signal Sw(n)
 *    which is obtained by filtering the input speech signal through
 *    the weighting filter
 *
 *    W(z) = A(z/g1) / A(z/g2)
 *
 *    That is, in a subframe of size L, the weighted speech is given by:
 *
 *                    10                           10
 *    Sw(n) = S(n) + SUM[a(i) * g1(i) * S(n-i)] - SUM[a(i) * g2(i) * Sw(n-i)],
 *                   i=1                          i=1
 *    n = 0, ..., L-1
 *
 * Returns:
 *    void
 */
static Word32 pre_big( enum Mode mode, const Float32 gamma1[], const Float32
      gamma1_12k2[], const Float32 gamma2[], Float32 A_t[], Word16 frame_offset,
      Float32 speech[], Float32 mem_w[], Float32 wsp[] )
{
   Float32 Ap1[MP1], Ap2[MP1];
   Word32 offset, i;


   /* A(z) with spectral expansion */
   const Float32 *g1;


   g1 = gamma1_12k2;

   offset = 0;

   if ( frame_offset > 0 ) {
      offset = MP1 << 1;
   }

   /* process two subframes (which form the "big" subframe) */
   for ( i = 0; i < 2; i++ ) {
      /* a(i) * g1(i) */
      Weight_Ai( &A_t[offset], g1, Ap1 );

      /* a(i) * g2(i) */
      Weight_Ai( &A_t[offset], gamma2, Ap2 );

      /*
       *       10
       *  S(n) + SUM[a(i) * g1(i) * S(n-i)]
       *       i=1
       */
      Residu( Ap1, &speech[frame_offset], &wsp[frame_offset] );

      /*
       *          10                            10
       *  S(n) + SUM[a(i) * g1(i) * S(n-i)]    SUM[a(i) * g2(i) * Sn(n-i)]
       *         i=1                           i=1
       */
      Syn_filt( Ap2, &wsp[frame_offset], &wsp[frame_offset], mem_w, 1 );
      offset += MP1;
      frame_offset += L_SUBFR;
   }
   return 0;
}


/*
 * comp_corr
 *
 *
 * Parameters:
 *    sig               I: signal
 *    L_frame           I: length of frame to compute pitch
 *    lag_max           I: maximum lag
 *    lag_min           I: minimum lag
 *    corr              O: correlation of selected lag
 *
 * Function:
 *    Calculate all correlations in a given delay range.
 *
 * Returns:
 *    void
 */
static void comp_corr( Float32 sig[], Word32 L_frame, Word32 lag_max, Word32
      lag_min, Float32 corr[] )
{
   Word32 i, j;
   Float32 *p, *p1;
   Float32 T0;


   for ( i = lag_max; i >= lag_min; i-- ) {
      p = sig;
      p1 = &sig[ - i];
      T0 = 0.0F;

      for ( j = 0; j < L_frame; j = j + 40, p += 40, p1 += 40 ) {
         T0 += p[0] * p1[0] + p[1] * p1[1] + p[2] * p1[2] + p[3] * p1[3];
         T0 += p[4] * p1[4] + p[5] * p1[5] + p[6] * p1[6] + p[7] * p1[7];
         T0 += p[8] * p1[8] + p[9] * p1[9] + p[10] * p1[10] + p[11] * p1[11];
         T0 += p[12] * p1[12] + p[13] * p1[13] + p[14] * p1[14] + p[15] * p1[15]
         ;
         T0 += p[16] * p1[16] + p[17] * p1[17] + p[18] * p1[18] + p[19] * p1[19]
         ;
         T0 += p[20] * p1[20] + p[21] * p1[21] + p[22] * p1[22] + p[23] * p1[23]
         ;
         T0 += p[24] * p1[24] + p[25] * p1[25] + p[26] * p1[26] + p[27] * p1[27]
         ;
         T0 += p[28] * p1[28] + p[29] * p1[29] + p[30] * p1[30] + p[31] * p1[31]
         ;
         T0 += p[32] * p1[32] + p[33] * p1[33] + p[34] * p1[34] + p[35] * p1[35]
         ;
         T0 += p[36] * p1[36] + p[37] * p1[37] + p[38] * p1[38] + p[39] * p1[39]
         ;
      }
      corr[ - i] = T0;
   }
   return;
}


/*
 * vad_tone_detection
 *
 *
 * Parameters:
 *    st->tone          B: flags indicating presence of a tone
 *    T0                I: autocorrelation maxima
 *    t1                I: energy
 *
 * Function:
 *    Set tone flag if pitch gain is high.
 *    This is used to detect signaling tones and other signals
 *    with high pitch gain.
 *
 * Returns:
 *    void
 */
#ifndef VAD2
static void vad_tone_detection( vadState *st, Float32 T0, Float32 t1 )
{
   if ( ( t1 > 0 ) && ( T0 > t1 * TONE_THR ) ) {
      st->tone = st->tone | 0x00004000;
   }
}
#endif

/*
 * Lag_max
 *
 *
 * Parameters:
 *    vadSt          B: vad structure
 *    corr           I: correlation vector
 *    sig            I: signal
 *    L_frame        I: length of frame to compute pitch
 *    lag_max        I: maximum lag
 *    lag_min        I: minimum lag
 *    cor_max        O: maximum correlation
 *    dtx            I: dtx on/off
 *
 * Function:
 *    Compute the open loop pitch lag.
 *
 * Returns:
 *    p_max             lag found
 */
#ifdef VAD2
static Word16 Lag_max( Float32 corr[], Float32 sig[], Word16 L_frame,
		       Word32 lag_max, Word32 lag_min, Float32 *cor_max,
		       Word32 dtx, Float32 *rmax, Float32 *r0 )
#else
static Word16 Lag_max( vadState *vadSt, Float32 corr[], Float32 sig[], Word16
      L_frame, Word32 lag_max, Word32 lag_min, Float32 *cor_max, Word32 dtx )
#endif
{
   Float32 max, T0;
   Float32 *p;
   Word32 i, j, p_max;


   max = -FLT_MAX;
   p_max = lag_max;

   for ( i = lag_max, j = ( PIT_MAX - lag_max - 1 ); i >= lag_min; i--, j-- ) {
      if ( corr[ - i] >= max ) {
         max = corr[ - i];
         p_max = i;
      }
   }

   /* compute energy for normalization */
   T0 = 0.0F;
   p = &sig[ - p_max];

   for ( i = 0; i < L_frame; i++, p++ ) {
      T0 += *p * *p;
   }

   if ( dtx ) {
#ifdef VAD2
     *rmax = max;
     *r0 = T0;
#else
     /* check tone */
     vad_tone_detection( vadSt, max, T0 );
#endif
   }

   if ( T0 > 0.0F )
      T0 = 1.0F / ( Float32 )sqrt( T0 );
   else
      T0 = 0.0F;

   /* max = max/sqrt(energy) */
   max *= T0;
   *cor_max = max;
   return( ( Word16 )p_max );
}


/*
 * hp_max
 *
 *
 * Parameters:
 *    corr           I: correlation vector
 *    sig            I: signal
 *    L_frame        I: length of frame to compute pitch
 *    lag_max        I: maximum lag
 *    lag_min        I: minimum lag
 *    cor_hp_max     O: max high-pass filtered correlation
 *
 * Function:
 *    Find the maximum correlation of scal_sig[] in a given delay range.
 *
 *    The correlation is given by
 *       cor[t] = <scal_sig[n],scal_sig[n-t]>,  t=lag_min,...,lag_max
 *    The functions outputs the maximum correlation after normalization
 *    and the corresponding lag.
 *
 * Returns:
 *    void
 */
#ifndef VAD2
static void hp_max( Float32 corr[], Float32 sig[], Word32 L_frame, Word32
      lag_max, Word32 lag_min, Float32 *cor_hp_max )
{
   Float32 T0, t1, max;
   Float32 *p, *p1;
   Word32 i;


   max = -FLT_MAX;
   T0 = 0;

   for ( i = lag_max - 1; i > lag_min; i-- ) {
      /* high-pass filtering */
      T0 = ( ( corr[ - i] * 2 ) - corr[ - i-1] )-corr[ - i + 1];
      T0 = ( Float32 )fabs( T0 );

      if ( T0 >= max ) {
         max = T0;
      }
   }

   /* compute energy */
   p = sig;
   p1 = &sig[0];
   T0 = 0;

   for ( i = 0; i < L_frame; i++, p++, p1++ ) {
      T0 += *p * *p1;
   }
   p = sig;
   p1 = &sig[ - 1];
   t1 = 0;

   for ( i = 0; i < L_frame; i++, p++, p1++ ) {
      t1 += *p * *p1;
   }

   /* high-pass filtering */
   T0 = T0 - t1;
   T0 = ( Float32 )fabs( T0 );

   /* max/T0 */
   if ( T0 != 0 ) {
      *cor_hp_max = max / T0;
   }
   else {
      *cor_hp_max = 0;
   }
}
#endif

/*
 * vad_tone_detection_update
 *
 *
 * Parameters:
 *    st->tone          B: flags indicating presence of a tone
 *    one_lag_per_frame I: 1 open-loop lag is calculated per each frame
 *
 * Function:
 *    Update the tone flag register.
 *
 * Returns:
 *    void
 */
#ifndef VAD2
static void vad_tone_detection_update( vadState *st, Word16 one_lag_per_frame )
{
   /* Shift tone flags right by one bit */
   st->tone = st->tone >> 1;

   /*
    * If open-loop lag is calculated only once in each frame,
    * do extra update and assume that the other tone flag
    * of the frame is one.
    */
   if ( one_lag_per_frame != 0 ) {
      st->tone = st->tone >> 1;
      st->tone = st->tone | 0x00002000;
   }
}
#endif

/*
 * Pitch_ol
 *
 *
 * Parameters:
 *    mode           I: AMR mode
 *    vadSt          B: VAD state struct
 *    signal         I: signal used to compute the open loop pitch
 *                                                 [[-pit_max]:[-1]]
 *    pit_min        I: minimum pitch lag
 *    pit_max        I: maximum pitch lag
 *    L_frame        I: length of frame to compute pitch
 *    dtx            I: DTX flag
 *    idx            I: frame index
 *
 * Function:
 *    Compute the open loop pitch lag.
 *
 *    Open-loop pitch analysis is performed twice per frame (each 10 ms)
 *    to find two estimates of the pitch lag in each frame.
 *    Open-loop pitch analysis is performed as follows.
 *    In the first step, 3 maxima of the correlation:
 *
 *          79
 *    O(k) = SUM Sw(n)*Sw(n-k)
 *          n=0
 *
 *    are found in the three ranges:
 *       pit_min     ...      2*pit_min-1
 *       2*pit_min   ...      4*pit_min-1
 *       4*pit_min   ...      pit_max
 *
 *    The retained maxima O(t(i)), i = 1, 2, 3, are normalized by dividing by
 *
 *    SQRT[SUM[POW(Sw(n-t(i)), 2]], i = 1, 2, 3,
 *         n
 *
 *    respectively.
 *    The normalized maxima and corresponding delays are denoted by
 *    (M(i), t(i)), i = 1, 2, 3. The winner, Top, among the three normalized
 *    correlations is selected by favouring the delays with the values
 *    in the lower range. This is performed by weighting the normalized
 *    correlations corresponding to the longer delays. The best
 *    open-loop delay Top is determined as follows:
 *
 *    Top = t(1)
 *    M(Top) = M(1)
 *    if M(2) > 0.85 * M(Top)
 *       M(Top) = M(2)
 *       Top = t(2)
 *    end
 *    if M(3) > 0.85 * M(Top)
 *       M(Top) = M(3)
 *       Top = t(3)
 *    end
 *
 * Returns:
 *    void
 */
static Word32 Pitch_ol( enum Mode mode, vadState *vadSt, Float32 signal[],
      Word32 pit_min, Word32 pit_max, Word16 L_frame, Word32 dtx, Word16 idx )
{
   Float32 corr[PIT_MAX + 1];
   Float32 max1, max2, max3, p_max1, p_max2, p_max3;
   Float32 *corr_ptr;
   Word32 i, j;
#ifdef VAD2
   Float32 r01, r02, r03;
   Float32 rmax1, rmax2, rmax3;
#else
   Float32 corr_hp_max;
#endif


#ifndef VAD2
   if ( dtx ) {
      /* update tone detection */
      {
         vad_tone_detection_update( vadSt, 0 );
      }
   }
#endif

   corr_ptr = &corr[pit_max];

   /*        79             */
   /* O(k) = SUM Sw(n)*Sw(n-k)   */
   /*        n=0               */
   comp_corr( signal, L_frame, pit_max, pit_min, corr_ptr );

#ifdef VAD2
   /* Find a maximum for each section.	*/
   /* Maxima 1	*/
   j = pit_min << 2;
   p_max1 =
     Lag_max( corr_ptr, signal, L_frame, pit_max, j, &max1, dtx, &rmax1, &r01 );

   /* Maxima 2	*/
   i = j - 1;
   j = pit_min << 1;
   p_max2 = Lag_max( corr_ptr, signal, L_frame, i, j, &max2, dtx, &rmax2, &r02 );

   /* Maxima 3	*/
   i = j - 1;
   p_max3 =
     Lag_max( corr_ptr, signal, L_frame, i, pit_min, &max3, dtx, &rmax3, &r03 );
#else
   /* Find a maximum for each section.	*/
   /* Maxima 1	*/
   j = pit_min << 2;
   p_max1 = Lag_max( vadSt, corr_ptr, signal, L_frame, pit_max, j, &max1, dtx );

   /* Maxima 2 */
   i = j - 1;
   j = pit_min << 1;
   p_max2 = Lag_max( vadSt, corr_ptr, signal, L_frame, i, j, &max2, dtx );

   /* Maxima 3 */
   i = j - 1;
   p_max3 = Lag_max( vadSt, corr_ptr, signal, L_frame, i, pit_min, &max3, dtx );

   if ( dtx ) {
      if ( idx == 1 ) {
         /* calculate max high-passed filtered correlation of all lags */
         hp_max( corr_ptr, signal, L_frame, pit_max, pit_min, &corr_hp_max );

         /* update complex background detector */
         vadSt->best_corr_hp = corr_hp_max * 0.5F;
      }
   }
#endif

   /* The best open-loop delay */
   if ( ( max1 * 0.85F ) < max2 ) {
      max1 = max2;
      p_max1 = p_max2;
#ifdef VAD2
      if (dtx) {
	rmax1 = rmax2;
	r01 = r02;
      }
#endif
   }

   if ( ( max1 * 0.85F ) < max3 ) {
      p_max1 = p_max3;
#ifdef VAD2
      if (dtx) {
	rmax1 = rmax3;
	r01 = r03;
      }
#endif
   }
#ifdef VAD2
   if (dtx) {
     vadSt->Rmax += rmax1;   /* Save max correlation */
     vadSt->R0   += r01;     /* Save max energy */
   }
#endif
   return( Word32 )p_max1;
}
/*
 * ol_ltp
 *
 *
 * Parameters:
 *    mode              I: AMR mode
 *    vadSt             B: VAD state struct
 *    wsp               I: signal used to compute the OL pitch
 *    T_op              O: open loop pitch lag
 *    ol_gain_flg       I: OL gain flag
 *    old_T0_med        O: old Cl lags median
 *    wght_flg          I: weighting function flag
 *    ada_w             B:
 *    old_lags          I: history with old stored Cl lags
 *    ol_gain_flg       I: OL gain flag
 *    dtx               I: DTX flag
 *    idx               I: frame index
 *
 * Function:
 *    Compute the open loop pitch lag.
 *
 *    Open-loop pitch analysis is performed in order to simplify
 *    the pitch analysis and confine the closed-loop pitch search to
 *    a small number of lags around the open-loop estimated lags.
 *    Open-loop pitch estimation is based on the weighted speech signal Sw(n)
 *    which is obtained by filtering the input speech signal through
 *    the weighting filter W(z) = A(z/g1) / A(z/g2). That is,
 *    in a subframe of size L, the weighted speech is given by:
 *
 *                10
 *    Sw(n) = S(n) + SUM[ a(i) * g1(i) * S(n-i) ]
 *                i=1
 *                   10
 *                - SUM[ a(i) * g2(i) * Sw(n-i) ], n = 0, ..., L-1
 *                  i=1
 *
 * Returns:
 *    void
 */
static void ol_ltp( enum Mode mode, vadState *vadSt, Float32 wsp[], Word32 *T_op
      , Float32 ol_gain_flg[], Word32 *old_T0_med, Word16 *wght_flg, Float32 *ada_w
      , Word32 *old_lags, Word32 dtx, Word16 idx )
{
   if ( mode != MR102 ) {
      ol_gain_flg[0] = 0;
      ol_gain_flg[1] = 0;
   }

   
         *T_op = Pitch_ol( mode, vadSt, wsp, PIT_MIN_MR122, PIT_MAX, L_FRAME_BY2
               , dtx, idx );
  
}


/*
 * subframePreProc
 *
 *
 * Parameters:
 *    mode           I: AMR mode
 *    gamma1         I: spectral exp. factor 1
 *    gamma1_12k2    I: spectral exp. factor 1 for EFR
 *    gamma2         I: spectral exp. factor 2
 *    A              I: A(z) unquantized for the 4 subframes
 *    Aq             I: A(z)   quantized for the 4 subframes
 *    speech         I: speech segment
 *    mem_err        I: pointer to error signal
 *    mem_w0         I: memory of weighting filter
 *    zero           I: pointer to zero vector
 *    ai_zero        O: history of weighted synth. filter
 *    exc            O: long term prediction residual
 *    h1             O: impulse response
 *    xn             O: target vector for pitch search
 *    res2           O: long term prediction residual
 *    error          O: error of LPC synthesis filter
 *
 * Function:
 *    Subframe preprocessing
 *
 *    Impulse response computation:
 *       The impulse response, h(n), of the weighted synthesis filter
 *
 *       H(z) * W(z) = A(z/g1) / ( A'(z) * A(z/g2) )
 *
 *       is computed each subframe. This impulse response is needed for
 *       the search of adaptive and fixed codebooks. The impulse response h(n)
 *       is computed by filtering the vector of coefficients of
 *       the filter A(z/g1) extended by zeros through the two filters
 *       1/A'(z) and 1/A(z/g2).
 *
 *    Target signal computation:
 *       The target signal for adaptive codebook search is usually computed
 *       by subtracting the zero input response of
 *       the weighted synthesis filter H(z) * W(z) from the weighted
 *       speech signal Sw(n). This is performed on a subframe basis.
 *       An equivalent procedure for computing the target signal is
 *       the filtering of the LP residual signal res(n) through
 *       the combination of synthesis filter 1/A'(z) and
 *       the weighting filter A(z/g1)/A(z/g2). After determining
 *       the excitation for the subframe, the initial states of
 *       these filters are updated by filtering the difference between
 *       the LP residual and excitation.
 *
 *       The residual signal res(n) which is needed for finding
 *       the target vector is also used in the adaptive codebook search
 *       to extend the past excitation buffer. This simplifies
 *       the adaptive codebook search procedure for delays less than
 *       the subframe size of 40. The LP residual is given by:
 *
 *                        10
 *       res(n) = S(n) + SUM[A'(i)* S(n-i)
 *                       i=1
 *
 * Returns:
 *    void
 */
static void subframePreProc( enum Mode mode, const Float32 gamma1[], const
      Float32 gamma1_12k2[], const Float32 gamma2[], Float32 *A, Float32 *Aq,
      Float32 *speech, Float32 *mem_err, Float32 *mem_w0, Float32 *zero, Float32
      ai_zero[], Float32 *exc, Float32 h1[], Float32 xn[], Float32 res2[],
      Float32 error[] )
{
   Float32 Ap1[MP1];   /* weighted LPC coefficients */
   Float32 Ap2[MP1];   /* weighted LPC coefficients */
   const Float32 *g1;


   /* mode specific pointer to gamma1 values */
   g1 = gamma1;

   if ( ( mode == MR122 ) || ( mode == MR102 ) ) {
      g1 = gamma1_12k2;
   }

   /* Find the weighted LPC coefficients for the weighting filter. */
   Weight_Ai( A, g1, Ap1 );
   Weight_Ai( A, gamma2, Ap2 );

   /*
    * Compute impulse response, h1[],
    * of weighted synthesis filter A(z/g1)/A(z/g2)
    */
   memcpy( ai_zero, Ap1, MP1 <<2 );
   Syn_filt( Aq, ai_zero, h1, zero, 0 );
   Syn_filt( Ap2, h1, h1, zero, 0 );

   /*
    * Find the target vector for pitch search:
    */
   /* LP residual */
   Residu( Aq, speech, res2 );
   memcpy( exc, res2, L_SUBFR <<2 );

   /* Synthesis filter */
   Syn_filt( Aq, exc, error, mem_err, 0 );
   Residu( Ap1, error, xn );

   /* target signal xn[] */
   Syn_filt( Ap2, xn, xn, mem_w0, 0 );
}


/*
 * getRange
 *
 *
 * Parameters:
 *    T0                I: integer pitch
 *    delta_low         I: search start offset
 *    delta_range       I: search range
 *    pitmin            I: minimum pitch
 *    pitmax            I: maximum pitch
 *    T0_min            I: search range minimum
 *    T0_max            I: search range maximum
 *
 * Function:
 *    Sets range around open-loop pitch or integer pitch of last subframe
 *
 *    Takes integer pitch T0 and calculates a range around it with
 *    T0_min = T0-delta_low and T0_max = (T0-delta_low) + delta_range
 *    T0_min and T0_max are bounded by pitmin and pitmax
 *
 * Returns:
 *    void
 */
static void getRange( Word32 T0, Word16 delta_low, Word16 delta_range,
      Word16 pitmin, Word16 pitmax, Word32 *T0_min, Word32 *T0_max )
{
   *T0_min = T0 - delta_low;

   if ( *T0_min < pitmin ) {
      *T0_min = pitmin;
   }
   *T0_max = *T0_min + delta_range;

   if ( *T0_max > pitmax ) {
      *T0_max = pitmax;
      *T0_min = *T0_max - delta_range;
   }
}


/*
 * Norm_Corr
 *
 *
 * Parameters:
 *    exc         I: excitation buffer                      [L_SUBFR]
 *    xn          I: target vector                          [L_SUBFR]
 *    h           I: impulse response of synthesis and weighting filters
 *                                                          [L_SUBFR]
 *    t_min       I: interval to compute normalized correlation
 *    t_max       I: interval to compute normalized correlation
 *    corr_norm   O: Normalized correlation                 [wT_min-wT_max]
 *
 * Function:
 *    Normalized correlation
 *
 *    The closed-loop pitch search is performed by minimizing
 *    the mean-square weighted error between the original and
 *    synthesized speech. This is achieved by maximizing the term:
 *
 *            39                           39
 *    R(k) = SUM[ X(n) * Yk(n)) ] / SQRT[ SUM[ Yk(n) * Yk(n)] ]
 *           n=0                          n=0
 *
 *    where X(n) is the target signal and Yk(n) is the past filtered
 *    excitation at delay k (past excitation convolved with h(n) ).
 *    The search range is limited around the open-loop pitch.
 *
 *    The convolution Yk(n) is computed for the first delay t_min in
 *    the searched range, and for the other delays in the search range
 *    k = t_min + 1, ..., t_max, it is updated using the recursive relation:
 *
 *    Yk(n) = Yk-1(n-1) + u(-k) * h(n),
 *
 *    where u(n), n = -( 143 + 11 ), ..., 39, is the excitation buffer.
 *    Note that in search stage, the samples u(n), n = 0, ..., 39,
 *    are not known, and they are needed for pitch delays less than 40.
 *    To simplify the search, the LP residual is copied to u(n) in order
 *    to make the relation in above equation valid for all delays.
 *
 * Returns:
 *    void
 */
static void Norm_Corr( Float32 exc[], Float32 xn[], Float32 h[], Word32 t_min,
      Word32 t_max, Float32 corr_norm[] )
{
   Float32 exc_temp[L_SUBFR];
   Float32 *p_exc;
   Float32 corr, norm;
   Float32 sum;
   Word32 i, j, k;


   k = -t_min;
   p_exc = &exc[ - t_min];

   /* compute the filtered excitation for the first delay t_min */
   /* convolution Yk(n) */
   for ( j = 0; j < L_SUBFR; j++ ) {
      sum = 0;

      for ( i = 0; i <= j; i++ ) {
         sum += p_exc[i] * h[j - i];
      }
      exc_temp[j] = sum;
   }

   /* loop for every possible period */
   for ( i = t_min; i <= t_max; i++ ) {
      /*        39                     */
      /* SQRT[ SUM[ Yk(n) * Yk(n)] ]   */
      /*       n=0                     */
      norm = (Float32)Dotproduct40( exc_temp, exc_temp );

      if ( norm == 0 )
         norm = 1.0;
      else
         norm = ( Float32 )( 1.0 / ( sqrt( norm ) ) );

      /*        39                  */
      /* SQRT[ SUM[ X(n) * Yk(n)] ] */
      /*       n=0                  */
      corr = (Float32)Dotproduct40( xn, exc_temp );

      /* R(k) */
      corr_norm[i] = corr * norm;

      /* modify the filtered excitation exc_tmp[] for the next iteration */
      if ( i != t_max ) {
         k--;

         for ( j = L_SUBFR - 1; j > 0; j-- ) {
            /* Yk(n) = Yk-1(n-1) + u(-k) * h(n) */
            exc_temp[j] = exc_temp[j - 1] + exc[k] * h[j];
         }
         exc_temp[0] = exc[k];
      }
   }
}


/*
 * Interpol_3or6
 *
 *
 * Parameters:
 *    x                 I: input vector
 *    frac              I: fraction  (-2..2 for 3*, -3..3 for 6*)
 *    flag3             I: if set, upsampling rate = 3 (6 otherwise)
 *
 * Function:
 *    Interpolating the normalized correlation with 1/3 or 1/6 resolution.
 *
 *    The interpolation is performed using an FIR filter b24
 *    based on a Hamming windowed sin(x)/x function truncated at ?3
 *    and padded with zeros at ?4 (b24(24) = 0). The filter has its
 *    cut-off frequency (-3 dB) at 3 600 Hz in the over-sampled domain.
 *    The interpolated values of R(k) for the fractions -3/6 to 3/6
 *    are obtained using the interpolation formula:
 *
 *              3                            3
 *    R(k)t = SUM[ R(k-i) * b24(t+i*6) ] + SUM [ R(k+1+i) * b24(6-t+i*6) ],
 *            i=0                          i=0
 *    t = 0, ..., 5,
 *
 *    where t = 0, ..., 5, corresponds to the fractions
 *    0, 1/6, 2/6, 3/6, -2/6, and -1/6, respectively. Note that it is
 *    necessary to compute the correlation terms using a range t_min - 4,
 *    t_max + 4, to allow for the proper interpolation.
 *
 * Returns:
 *    s                 interpolated value
 */
static Float32 Interpol_3or6( Float32 *x, Word32 frac, Word16 flag3 )
{
   Float32 s;
   Float32 *x1, *x2;
   const Float32 *c1, *c2;
   Word32 i, k;


   if ( flag3 != 0 ) {
      /* inter_3[k] = b60[2*k] -> k' = 2*k */
      frac <<= 1;
   }

   if ( frac < 0 ) {
      frac += UP_SAMP_MAX;
      x--;
   }
   x1 = &x[0];
   x2 = &x[1];
   c1 = &b24[frac];
   c2 = &b24[UP_SAMP_MAX - frac];
   s = 0;

   for ( i = 0, k = 0; i < L_INTER_SRCH; i++, k += UP_SAMP_MAX ) {
      /* R(k-i) * b24(t+i*6) */
      s += x1[ - i] * c1[k];

      /* R(k+1+i) * b24(6-t+i*6) */
      s += x2[i] * c2[k];
   }
   return s;
}


/*
 * searchFrac
 *
 *
 * Parameters:
 *    lag               B: integer pitch
 *    frac              B: start point of search - fractional pitch
 *    last_frac         I: endpoint of search
 *    corr              I: normalized correlation
 *    flag3             I: if set, upsampling rate = 3 (6 otherwise)
 *
 * Function:
 *    Find fractional pitch
 *
 *    The function interpolates the normalized correlation at the
 *    fractional positions around lag T0. The position at which the
 *    interpolation function reaches its maximum is the fractional pitch.
 *    Starting point of the search is frac, end point is last_frac.
 *    frac is overwritten with the fractional pitch.
 *
 * Returns:
 *    void
 */
static void searchFrac( Word32 *lag, Word32 *frac, Word16 last_frac, Float32
      corr[], Word16 flag3 )
{
   Float32 max, corr_int;
   Word32 i;


   /*
    * Test the fractions around T0 and choose the one which maximizes
    * the interpolated normalized correlation.
    */
   max = Interpol_3or6( &corr[ * lag], *frac, flag3 );

   for ( i = *frac + 1; i <= last_frac; i++ ) {
      corr_int = Interpol_3or6( &corr[ * lag], i, flag3 );

      if ( corr_int > max ) {
         max = corr_int;
         *frac = i;
      }
   }

   if ( flag3 == 0 ) {
      /* Limit the fraction value in the interval [-2,-1,0,1,2,3] */
      if ( *frac == -3 ) {
         *frac = 3;
         *lag -= 1;
      }
   }
   else {
      /* limit the fraction value between -1 and 1 */
      if ( *frac == -2 ) {
         *frac = 1;
         *lag -= 1;
      }

      if ( *frac == 2 ) {
         *frac = -1;
         *lag += 1;
      }
   }
}


/*
 * Enc_lag3
 *
 *
 * Parameters:
 *    T0             I: Pitch delay
 *    T0_frac        I: Fractional pitch delay
 *    T0_prev        I: Integer pitch delay of last subframe
 *    T0_min         I: minimum of search range
 *    T0_max         I: maximum of search range
 *    delta_flag     I: Flag for 1st (or 3rd) subframe
 *    flag4          I: Flag for encoding with 4 bits
 *
 * Function:
 *    Encoding of fractional pitch lag with 1/3 resolution.
 *
 * Returns:
 *    index             index of encoding
 */
static Word32 Enc_lag3( Word32 T0, Word32 T0_frac, Word32 T0_prev, Word32 T0_min
      , Word32 T0_max, Word16 delta_flag, Word16 flag4 )
{
   Word32 index, i, tmp_ind, uplag, tmp_lag;


   /* if 1st or 3rd subframe */
   if ( delta_flag == 0 ) {
      /* encode pitch delay (with fraction) */
      if ( T0 <= 85 ) {
         index = T0 * 3 - 58 + T0_frac;
      }
      else {
         index = T0 + 112;
      }
   }

   /* if second or fourth subframe */
   else {
      if ( flag4 == 0 ) {
         /* 'normal' encoding: either with 5 or 6 bit resolution */
         index = 3 * ( T0 - T0_min ) + 2 + T0_frac;
      }
      else {
         /* encoding with 4 bit resolution */
         tmp_lag = T0_prev;

         if ( ( tmp_lag - T0_min ) > 5 )
            tmp_lag = T0_min + 5;

         if ( ( T0_max - tmp_lag ) > 4 )
            tmp_lag = T0_max - 4;
         uplag = T0 + T0 + T0 + T0_frac;
         i = tmp_lag - 2;
         tmp_ind = i + i + i;

         if ( tmp_ind >= uplag ) {
            index = ( T0 - tmp_lag ) + 5;
         }
         else {
            i = tmp_lag + 1;
            i = i + i + i;

            if ( i > uplag ) {
               index = ( uplag - tmp_ind ) + 3;
            }
            else {
               index = ( T0 - tmp_lag ) + 11;
            }
         }
      }   /* end if (encoding with 4 bit resolution) */
   }   /* end if (second of fourth subframe) */
   return index;
}


/*
 * Enc_lag6
 *
 *
 * Parameters:
 *    T0             I: Pitch delay
 *    T0_frac        I: Fractional pitch delay
 *    T0_min         I: minimum of search range
 *    delta_flag     I: Flag for 1st (or 3rd) subframe
 *
 * Function:
 *    Encoding of fractional pitch lag with 1/6 resolution.
 *
 *    First and third subframes:
 *       The pitch range is divided as follows:
 *          17 3/6  to   94 3/6   resolution 1/6
 *          95      to   143      resolution 1
 *       The period is encoded with 9 bits.
 *       For the range with fractions:
 *          index = (T-17)*6 + frac - 3;
 *          where T=[17..94] and frac=[-2,-1,0,1,2,3]
 *       and for the integer only range
 *          index = (T - 95) + 463;
 *          where T=[95..143]
 *    Second and fourth subframes:
 *       For the 2nd and 4th subframes a resolution of 1/6 is always used,
 *       and the search range is relative to the lag in previous subframe.
 *       If T0 is the lag in the previous subframe then
 *       t_min=T0-5   and  t_max=T0+4   and  the range is given by
 *       (t_min-1) 3/6   to  (t_max) 3/6
 *       The period in the 2nd (and 4th) subframe is encoded with 6 bits:
 *          index = (T-(t_min-1))*6 + frac - 3;
 *          where T=[t_min-1..t_max] and frac=[-2,-1,0,1,2,3]
 *    Note that only 61 values are used. If the decoder receives 61, 62,
 *    or 63 as the relative pitch index, it means that a transmission
 *    error occurred and the pitch from previous subframe should be used.
 *
 * Returns:
 *    index             index of encoding
 */
static Word32 Enc_lag6( Word32 T0, Word32 T0_frac, Word32 T0_min, Word16
      delta_flag )
{
   Word32 index;


   /* if 1st or 3rd subframe */
   if ( delta_flag == 0 ) {
      /* encode pitch delay (with fraction) */
      if ( T0 <= 94 ) {
         /* index = (t-17)*6 + frac - 3 */
         index = T0 * 6 - 105 + T0_frac;
      }
      else {
         /* index = (t - 95) + 463 */
         index = T0 + 368;
      }
   }

   /* if second or fourth subframe */
   else {
      /* index = (t-(t_min-1))*6 + frac - 3 */
      index = 6 * ( T0 - T0_min ) + 3 + T0_frac;
   }
   return index;
}


/*
 * Pitch_fr
 *
 *
 * Parameters:
 *    T0_prev_subframe  B: integer pitch lag of previous sub-frame
 *    mode              I: codec mode
 *    T_op              I: open-loop pitch estimations for
 *                         the 2 big subframes [2]
 *    exc               I: excitation buffer
 *    xn                I: target vector
 *    h                 I: impulse response of synthesis
 *                         and weighting filters
 *    i_subfr           I: subframe number
 *    pit_frac          O: pitch period (fractional)
 *    resu3             O: subsample resolution 1/3 (=1) or 1/6 (=0)
 *    ana_index         O: index of encoding
 *
 * Function:
 *    Closed-loop pitch search
 *
 *    In the first and third subframes, a fractional pitch delay is used
 *    with resolutions: 1/6 in the range [17 3/6, 94 3/6] and integers only
 *    in the range [95, 143]. For the second and fourth subframes,
 *    a pitch resolution of 1/6 is always used in
 *    the range [T1 - 5 3/6, T1 + 4 /3/6], where T1 is nearest integer to
 *    the fractional pitch lag of the previous (1st or 3rd) subframe,
 *    bounded by 18...143.
 *
 *    Closed-loop pitch analysis is performed around
 *    the open-loop pitch estimates on a subframe basis.
 *    In the first (and third) subframe the range Top?,
 *    bounded by 18...143, is searched. For the other subframes,
 *    closed-loop pitch analysis is performed around the integer pitch
 *    selected in the previous subframe, as described above.
 *    The pitch delay is encoded with 9 bits in the first and
 *    third subframes and the relative delay of the other subframes
 *    is encoded with 6 bits.
 *
 *    The closed-loop pitch search is performed by minimizing
 *    the mean-square weighted error between the original and
 *    synthesized speech. This is achieved by maximizing the term:
 *
 *            39                           39
 *    R(k) = SUM[ X(n) * Yk(n)) ] / SQRT[ SUM[ Yk(n) * Yk(n)] ]
 *           n=0                          n=0
 *
 *    where X(n) is the target signal and Yk(n) is the past filtered
 *    excitation at delay k (past excitation convolved with h(n) ).
 *
 *    Once the optimum integer pitch delay is determined, the fractions
 *    from -3/6 to 3/6 with a step of 1/6 around that integer are tested.
 *    The fractional pitch search is performed by interpolating
 *    the normalized correlation R(k) and searching for its maximum.
 *    The interpolation is performed using an FIR filter b24
 *    based on a Hamming windowed sin(x)/x function truncated at ?3
 *    and padded with zeros at ?4 (b24(24) = 0). The filter has its
 *    cut-off frequency (-3 dB) at 3 600 Hz in the over-sampled domain.
 *    The interpolated values of R(k) for the fractions -3/6 to 3/6
 *    are obtained using the interpolation formula:
 *
 *              3                            3
 *    R(k)t = SUM[ R(k-i) * b24(t+i*6) ] + SUM [ R(k+1+i) * b24(6-t+i*6) ],
 *            i=0                          i=0
 *    t = 0, ..., 5,
 *
 *    where t = 0, ..., 5, corresponds to the fractions
 *    0, 1/6, 2/6, 3/6, -2/6, and -1/6, respectively. Note that it is
 *    necessary to compute the correlation terms using a range t_min -4,
 *    t_max + 4, to allow for the proper interpolation.
 *
 * Returns:
 *    lag             closed-loop pitch lag
 */
static Word32 Pitch_fr( Word32 *T0_prev_subframe, enum Mode mode, Word32 T_op[],
      Float32 exc[], Float32 xn[], Float32 h[], Word16 i_subfr, Word32 *pit_frac
      , Word16 *resu3, Word32 *ana_index )
{
   Float32 corr_v[40];
   Float32 max;
   Float32 *corr;
   Word32 i, t_min, t_max, T0_min, T0_max;
   Word32 lag, frac;
   Word16 max_frac_lag, flag3, flag4, last_frac;
   Word16 delta_int_low, delta_int_range, delta_frc_low, delta_frc_range;
   Word16 pit_min;
   Word16 frame_offset;
   Word16 delta_search;


   /* set mode specific variables */
   max_frac_lag = mode_dep_parm[mode].max_frac_lag;
   flag3 = mode_dep_parm[mode].flag3;
   frac = mode_dep_parm[mode].first_frac;
   last_frac = mode_dep_parm[mode].last_frac;
   delta_int_low = mode_dep_parm[mode].delta_int_low;
   delta_int_range = mode_dep_parm[mode].delta_int_range;
   delta_frc_low = mode_dep_parm[mode].delta_frc_low;
   delta_frc_range = mode_dep_parm[mode].delta_frc_range;
   pit_min = mode_dep_parm[mode].pit_min;

   /* decide upon full or differential search */
   delta_search = 1;

   if ( ( i_subfr == 0 ) || ( i_subfr == L_FRAME_BY2 ) ) {
      /* Subframe 1 and 3 */
      if ( ( ( mode != MR475 ) && ( mode != MR515 ) ) || ( i_subfr !=
            L_FRAME_BY2 ) ) {
         /*
          * set T0_min, T0_max for full search
          * this is *not* done for mode MR475, MR515 in subframe 3
          */
         delta_search = 0;   /* no differential search */

         /*
          * calculate index into T_op which contains the open-loop
          * pitch estimations for the 2 big subframes
          */
         frame_offset = 1;

         if ( i_subfr == 0 )
            frame_offset = 0;

         /*
          * get T_op from the corresponding half frame and
          * set T0_min, T0_max
          */
         getRange( T_op[frame_offset], delta_int_low, delta_int_range, pit_min,
               PIT_MAX, &T0_min, &T0_max );
      }
   }
   else {
      /*
       * for Subframe 2 and 4
       * get range around T0 of previous subframe for delta search
       */
      getRange( *T0_prev_subframe, delta_frc_low, delta_frc_range, pit_min,
            PIT_MAX, &T0_min, &T0_max );
   }

   /* Find interval to compute normalized correlation */
   t_min = T0_min - L_INTER_SRCH;
   t_max = T0_max + L_INTER_SRCH;
   corr = &corr_v[ - t_min];

   /* Compute normalized correlation between target and filtered excitation */
   Norm_Corr( exc, xn, h, t_min, t_max, corr );

   /* Find integer pitch */
   max = corr[T0_min];
   lag = T0_min;

   for ( i = T0_min + 1; i <= T0_max; i++ ) {
      if ( corr[i] >= max ) {
         max = corr[i];
         lag = i;
      }
   }

   /* Find fractional pitch   */
   if ( ( delta_search == 0 ) && ( lag > max_frac_lag ) ) {
      /*
       * full search and integer pitch greater than max_frac_lag
       * fractional search is not needed, set fractional to zero
       */
      frac = 0;
   }
   else {

         /* test the fractions around T0 */
         searchFrac( &lag, &frac, last_frac, corr, flag3 );
   }

   /*
    *  encode pitch
    */
   if ( flag3 != 0 ) {
      /*
       * flag4 indicates encoding with 4 bit resolution;
       * this is needed for mode MR475, MR515 and MR59
       */
      flag4 = 0;

      /* encode with 1/3 subsample resolution */
      *ana_index = Enc_lag3( lag, frac, *T0_prev_subframe, T0_min, T0_max,
            delta_search, flag4 );
   }
   else {
      /* encode with 1/6 subsample resolution */
      *ana_index = Enc_lag6( lag, frac, T0_min, delta_search );
   }

   /*
    *  update state variables
    */
   *T0_prev_subframe = lag;

   /*
    * update output variables
    */
   *resu3 = flag3;
   *pit_frac = frac;
   return( lag );
}



static void Pred_lt_3or6_fixed( Word32 exc[], Word32 T0, Word32 frac, Word32 flag3 )
{
   Word32 s, i;
   Word32 *x0, *x1, *x2;
   const Word32 *c1, *c2;

   x0 = &exc[ - T0];
   frac = -frac;

   if ( flag3 != 0 ) {
      frac <<= 1;   /* inter_3l[k] = inter6[2*k] -> k' = 2*k */
   }

   if ( frac < 0 ) {
      frac += 6;
      x0--;
   }
   c1 = &inter6[frac];
   c2 = &inter6[6 - frac];

   for ( i = 0; i < 40; i++ ) {
      x1 = x0++;
      x2 = x0;
      s = x1[0] * c1[0];
      s += x1[ - 1] * c1[6];
      s += x1[ - 2] * c1[12];
      s += x1[ - 3] * c1[18];
      s += x1[ - 4] * c1[24];
      s += x1[ - 5] * c1[30];
      s += x1[ - 6] * c1[36];
      s += x1[ - 7] * c1[42];
      s += x1[ - 8] * c1[48];
      s += x1[ - 9] * c1[54];
      s += x2[0] * c2[0];
      s += x2[1] * c2[6];
      s += x2[2] * c2[12];
      s += x2[3] * c2[18];
      s += x2[4] * c2[24];
      s += x2[5] * c2[30];
      s += x2[6] * c2[36];
      s += x2[7] * c2[42];
      s += x2[8] * c2[48];
      s += x2[9] * c2[54];
      exc[i] = ( s + 0x4000 ) >> 15;

   }
}


/*
 * G_pitch
 *
 *
 * Parameters:
 *    xn       I: Pitch target
 *    y1       I: Filtered adaptive codebook
 *    gCoeff   O: Correlations need for gain quantization
 *
 * Function:
 *    Compute the pitch (adaptive codebook) gain.
 *
 *    The adaptive codebook gain is given by
 *
 *       g = <x[], y[]> / <y[], y[]>
 *
 *    where x[] is the target vector, y[] is the filtered adaptive
 *    codevector, and <> denotes dot product.
 *
 * Returns:
 *    gain              gain saturated to 1.2
 */
static Float32 G_pitch( Float32 xn[], Float32 y1[], Float32 gCoeff[] )
{
   Float32 gain, sum;


   /* Compute scalar product <y1[],y1[]> */
   sum = (Float32)Dotproduct40( y1, y1 );

   /* Avoid case of all zeros */
   sum += 0.01F;
   gCoeff[0] = sum;

   /* Compute scalar product <xn[],y1[]> */
   sum = (Float32)Dotproduct40( xn, y1 );
   gCoeff[1] = sum;

   /* compute gain = xy/yy */
   gain = ( Float32 )( gCoeff[1] / gCoeff[0] );

   /* if(gain >1.2) gain = 1.2 */
   if ( gain < 0.0 )
      gain = 0.0F;

   if ( gain > 1.2 )
      gain = 1.2F;
   return( gain );
}


/*
 * check_gp_clipping
 *
 *
 * Parameters:
 *    gp          I: old pitch gains
 *    g_pitch     I: pitch gain
 *
 * Function:
 *    Verify that the sum of the last (N_FRAME+1) pitch gains is under
 *    a certain threshold.
 *
 * Returns:
 *    True or false
 */
static Word16 check_gp_clipping( Float32 *gp, Float32 g_pitch )
{
   Float32 sum;
   Word32 i;


   sum = g_pitch;

   for ( i = 0; i < N_FRAME; i++ ) {
      sum += gp[i];
   }

   if ( sum > 7.6F /*FGP_CLIP*/ ) {
      return 1;
   }
   else {
      return 0;
   }
}


/*
 * q_gain_pitch
 *
 *
 * Parameters:
 *    mode              I: codec mode
 *    gp_limit          I: pitch gain limit
 *    gain              B: Pitch gain (unquant/quant)
 *    gain_cand         O: pitch gain candidates (3),   MR795 only
 *    gain_cind         O: pitch gain cand. indices (3),MR795 only
 *
 * Function:
 *    Closed-loop pitch search
 *
 * Returns:
 *    index             index of quantization
 */
static Word16 q_gain_pitch( enum Mode mode, Float32 gp_limit, Float32 *gain,
      Float32 gain_cand[], Word32 gain_cind[] )
{
   Float32 err_min, err;
   Word32 i, index;


   err_min = ( Float32 )fabs( *gain - qua_gain_pitch[0] );
   index = 0;

   for ( i = 1; i < NB_QUA_PITCH; i++ ) {
      if ( qua_gain_pitch[i] <= gp_limit ) {
         err = ( Float32 )fabs( *gain - qua_gain_pitch[i] );

         if ( err < err_min ) {
            err_min = err;
            index = i;
         }
      }
   }

   {
      /* return the index and gain pitch found */
      *gain = qua_gain_pitch_MR122[index];
   }
   return( Word16 )index;
}


/*
 * cl_ltp
 *
 *
 * Parameters:
 *    T0_prev_subframe  B: Integer pitch lag of previous sub-frame
 *    gp                I: Gain history
 *    mode              I: Coder mode
 *    frame_offset      I: Offset to subframe
 *    T_op              I: Open loop pitch lags
 *    h1                I: Impulse response vector
 *    exc               B: Excitation vector
 *    res2              B: Long term prediction residual
 *    xn                I: Target vector for pitch search
 *    lsp_flag          I: LSP resonance flag
 *    xn2               O: Target vector for codebook search
 *    y1                O: Filtered adaptive excitation
 *    T0                O: Pitch delay (integer part)
 *    T0_frac           O: Pitch delay (fractional part)
 *    gain_pit          O: Pitch gain
 *    gCoeff[]          O: Correlations between xn, y1, & y2
 *    anap              O: Analysis parameters
 *    gp_limit          O: pitch gain limit
 *
 * Function:
 *    Closed-loop ltp search
 *
 *    Adaptive codebook search is performed on a subframe basis.
 *    It consists of performing closed-loop pitch search, and then computing
 *    the adaptive codevector by interpolating the past excitation at
 *    the selected fractional pitch lag.
 *    The adaptive codebook parameters (or pitch parameters) are
 *    the delay and gain of the pitch filter. In the adaptive codebook approach
 *    for implementing the pitch filter, the excitation is repeated for delays
 *    less than the subframe length. In the search stage, the excitation is
 *    extended by the LP residual to simplify the closed-loop search.
 *
 * Returns:
 *    void
 */
static void cl_ltp( Word32 *T0_prev_subframe, Float32 *gp, enum Mode mode,
      Word16 frame_offset, Word32 T_op[], Float32 *h1, Float32 *exc, Float32
      res2[], Float32 xn[], Word16 lsp_flag, Float32 xn2[], Float32 y1[], Word32
      *T0, Word32 *T0_frac, Float32 *gain_pit, Float32 gCoeff[], Word16 **anap,
      Float32 *gp_limit )
{
   Float32 s;
   Word32 i, n;
   Word16 gpc_flag, resu3;   /* flag for upsample resolution */

   Word32 exc_tmp[314];
   Word32 *exc_tmp_p;

   exc_tmp_p = exc_tmp + PIT_MAX + L_INTERPOL;


   /* Closed-loop fractional pitch search */
   *T0 = Pitch_fr( T0_prev_subframe, mode, T_op, exc, xn, h1, frame_offset,
         T0_frac, &resu3, &i );
   *( *anap )++ = ( Word16 )i;

   /*
    * Compute the adaptive codebook vector
    * using fixed point. This is required
    * to maintain encoder/decoder excitation
    * syncronisation
    */
   for (i = -(PIT_MAX + L_INTERPOL); i < 40; i++)
      exc_tmp_p[i] = (Word32)exc[i];

   Pred_lt_3or6_fixed( exc_tmp_p, *T0, *T0_frac, resu3 );

   for (i = -(PIT_MAX + L_INTERPOL); i < 40; i++)
      exc[i] = (Float32)exc_tmp_p[i];

   /*
    *   Convolve to get filtered adaptive codebook vector
    *  y[n] = sum_{i=0}^{n} x[i] h[n-i], n=0,...,L-1
    */
   for ( n = 0; n < L_SUBFR; n++ ) {
      s = 0;

      for ( i = 0; i <= n; i++ ) {
         s += exc[i] * h1[n - i];
      }
      y1[n] = s;
   }

   /* The adaptive codebook gain */
   *gain_pit = G_pitch( xn, y1, gCoeff );

   /* check if the pitch gain should be limit due to resonance in LPC filter */
   gpc_flag = 0;
   *gp_limit = 2.0F;

   if ( ( lsp_flag != 0 ) && ( *gain_pit > 0.95F ) ) {
      gpc_flag = check_gp_clipping( gp, *gain_pit );
   }

   /*
    * special for the MR475, MR515 mode; limit the gain to 0.85 to
    * cope with bit errors in the decoder in a better way.
    */
      if ( gpc_flag != 0 ) {
         *gp_limit = GP_CLIP;
         *gain_pit = GP_CLIP;
      }

      /*
       * 12k2 gain_pit is quantized here and not in gainQuant.
       */
      if ( mode == MR122 ) {

         *( *anap )++ = q_gain_pitch( MR122, *gp_limit, gain_pit, NULL, NULL );
      }

   /*
    * Update target vector for codebook search
    * Find LTP residual
    */
   for ( i = 0; i < L_SUBFR; i++ ) {
      xn2[i] = xn[i] - y1[i] * *gain_pit;
      res2[i] = res2[i] - exc[i] * *gain_pit;
   }
}


/*
 * DotProduct
 *
 *
 * Parameters:
 *    x                 I: first input
 *    y                 I: second input
 *    len               I: length of product
 *
 * Function:
 *    Computes dot product
 *
 * Returns:
 *    acc               dot product
 */
static Float32 DotProduct( Float32 *x, Float32 *y, Word32 len )
{
   Word32 i;
   Float32 acc;


   acc = 0.0F;

   for ( i = 0; i < len; i++ )
      acc += x[i] * y[i];
   return( acc );
}


/*
 * cor_h_x
 *
 *
 * Parameters:
 *    h                 I: impulse response of weighted synthesis filter
 *    x                 I: target
 *    dn                O: correlation between target and impulse response
 *
 * Function:
 *    Computes correlation between target signal and impulse response.
 *
 * Returns:
 *    void
 */
static void cor_h_x( Float32 h[], Float32 x[], Float32 dn[] )
{
   Word32 i;


   dn[0] = (Float32)Dotproduct40( h, x );

   for ( i = 1; i < L_CODE; i++ )
      dn[i] = (Float32)DotProduct( h, &x[i], L_CODE - i );
}



/*
 * cor_h
 *
 *
 * Parameters:
 *    h                I: h[]
 *    sign             I: sign information
 *    rr               O: correlations
 *
 * Function:
 *    Computes correlations of h[] needed for the codebook search,
 *    and includes the sign information into the correlations.
 *
 * Returns:
 *    void
 */
static void cor_h( Float32 h[], Float32 sign[], Float32 rr[][L_CODE] )
{
   Float32 sum;
   Float32 *prr, *ph, *ph_max;
   Float32 *rrj, *rri, *signi, *signj;
   Word32 ii, total_loops, four_loops;


   sum = 0.0F;

   /* Compute diagonal matrix of autocorrelation of h */
   rr[0][0] = (Float32)Dotproduct40( h, h );
   prr = &rr[39][39];
   ph = &h[0];
   ph_max = ph + 39;

   /*
    * speed optimization of code:
    * for (k=0; k<m; k++)
    * {
    * sum += h[k]*h[k];
    * rr[i][i] = sum;
    * i--;
    * }
    */
   do {
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
      sum += *ph * *ph;
      ph++;
      *prr = sum;
      prr -= 41;
   } while ( ph < ph_max );

   /*
    * Compute upper & bottom symmetric matrix of autocorrelation of h.
    */
   /* speed optimization of code:
    * for (ii=1; ii<L_CODE; ii++)
    * {
    * j = m;
    * i = j - ii;
    * sum = (Float32)0.0;
    * for ( k = 0; k < (L_CODE-ii); k++ )
    * {
    * sum += h[k]*h[k+ii];
    * rr[i][j] = rr[j][i] = (sum*sign[i]*sign[j]);
    * i--; j--;
    * }
    * }
    */
   ii = 1;

   for ( total_loops = 9; total_loops >= 0; total_loops-- ) {
      rrj = rri = &rr[39][39];
      rrj -= ii;
      rri = ( rri - 40 * ii );
      signi = signj = &sign[39];
      signi -= ii;
      sum = 0.0F;
      ph = &h[0];

      for ( four_loops = 0; four_loops < total_loops; four_loops++ ) {
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
      }
      sum += *ph * *( ph + ii );
      ph++;
      *rri = *rrj = sum * *signi * *signj;
      rri -= 41;
      rrj -= 41;
      signi--;
      signj--;
      sum += *ph * *( ph + ii );
      ph++;
      *rri = *rrj = sum * *signi * *signj;
      rri -= 41;
      rrj -= 41;
      signi--;
      signj--;
      sum += *ph * *( ph + ii );
      *rri = *rrj = sum * *signi * *signj;
      ii++;
      rrj = rri = &rr[39][39];
      rrj -= ii;
      rri = ( rri - 40 * ii );
      signi = signj = &sign[39];
      signi -= ii;
      sum = 0.0F;
      ph = &h[0];

      for ( four_loops = 0; four_loops < total_loops; four_loops++ ) {
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
      }
      sum += *ph * *( ph + ii );
      ph++;
      *rri = *rrj = sum * *signi * *signj;
      rri -= 41;
      rrj -= 41;
      signi--;
      signj--;
      sum += *ph * *( ph + ii );
      *rri = *rrj = sum * *signi * *signj;
      ii++;
      rrj = rri = &rr[39][39];
      rrj -= ii;
      rri = ( rri - 40 * ii );
      signi = signj = &sign[39];
      signi -= ii;
      sum = 0.0F;
      ph = &h[0];

      for ( four_loops = 0; four_loops < total_loops; four_loops++ ) {
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
      }
      sum += *ph * *( ph + ii );
      *rri = *rrj = sum * *signi * *signj;
      ii++;
      rrj = rri = &rr[39][39];
      rrj -= ii;
      rri = ( rri - 40 * ii );
      signi = signj = &sign[39];
      signi -= ii;
      sum = 0.0F;
      ph = &h[0];

      for ( four_loops = 0; four_loops < total_loops; four_loops++ ) {
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * ( *signi ) * ( *signj );
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
         sum += *ph * *( ph + ii );
         ph++;
         *rri = *rrj = sum * *signi * *signj;
         rri -= 41;
         rrj -= 41;
         signi--;
         signj--;
      }
      ii++;
   }
   return;
}

/*
 * set_sign12k2
 *
 *
 * Parameters:
 *    dn                B: correlation between target and h[]
 *    cn                I: residual after long term prediction
 *    sign              O: sign of dn[]
 *    pos_max           O: position of maximum correlation
 *    nb_track          I: number of tracks
 *    ipos              O: starting position for each pulse
 *    step              I: the step size in the tracks
 *
 * Function:
 *    Builds sign[] vector according to "dn[]" and "cn[]", and modifies
 *    dn[] to include the sign information (dn[i]=sign[i]*dn[i]).
 *    Also finds the position of maximum of correlation in each track
 *    and the starting position for each pulse.
 *
 *
 *                  cn[n]                     dn[n]
 *    b(n) = ----------------------- + -----------------------  ;n = 0,..,39
 *                   39                        39
 *            sqrt( SUM cn[i]*cn[i] )   sqrt( SUM dn[i]*dn[i] )
 *                  i=0                       i=0
 *
 *    sign[n] = sign(b[n])
 *
 *    d'[n] = sign[n] * d[n]
 *
 * Returns:
 *    void
 */
static void set_sign12k2( Float32 dn[], Float32 cn[], Float32 sign[], Word32
      pos_max[], Word16 nb_track, Word32 ipos[], Word16 step )
{
   Float32 b[L_CODE];
   Float32 val, cor, k_cn, k_dn, max, max_of_all, sum;
   Word32 i, j, pos = 0;


   /* calculate energy for normalization of cn[] and dn[] */
   sum = 0.01F;
   sum += (Float32)Dotproduct40( cn, cn );
   k_cn = ( Float32 )( 1 / sqrt( sum ) );
   sum = 0.01F;
   sum += (Float32)Dotproduct40( dn, dn );
   k_dn = ( Float32 )( 1 / sqrt( sum ) );

   for ( i = 0; i < L_CODE; i++ ) {
      val = dn[i];
      cor = ( k_cn * cn[i] ) + ( k_dn * val );

      /* sign = +1 */
      sign[i] = 1;

      if ( cor < 0 ) {
         /* sign = -1 */
         sign[i] = -1;
         cor = -cor;
         val = -val;
      }

      /* modify dn[] according to the fixed sign */
      dn[i] = val;
      b[i] = cor;
   }
   max_of_all = -1;

   for ( i = 0; i < nb_track; i++ ) {
      max = -1;

      for ( j = i; j < L_CODE; j += step ) {
         cor = b[j];
         val = cor - max;

         if ( val > 0 ) {
            max = cor;
            pos = j;
         }
      }

      /* store maximum correlation position in track */
      pos_max[i] = pos;
      val = max - max_of_all;

      if ( val > 0 ) {
         /* store maximum correlation of all tracks */
         max_of_all = max;

         /* starting position for i0 */
         ipos[0] = i;
      }
   }

   /* Set starting position of each pulse	*/
   pos = ipos[0];
   ipos[nb_track] = pos;

   for ( i = 1; i < nb_track; i++ ) {
      pos++;

      if ( pos >= nb_track ) {
         pos = 0;
      }
      ipos[i] = pos;
      ipos[i + nb_track] = pos;
   }
}

/*
 * search_10i40
 *
 *
 * Parameters:
 *    dn                I: correlation between target and h[]
 *    rr                I: matrix of autocorrelation
 *    ipos              I: starting position for each pulse
 *    pos_max           I: maximum of correlation position
 *    codvec            O: algebraic codebook vector
 *
 * Function:
 *    Search the best codevector; determine positions of the 10
 *    pulses in the 40-sample frame.
 *
 *    First, for each of the five tracks the pulse positions with maximum
 *    absolute values of b(n) are searched. From these the global maximum value
 *    for all the pulse positions is selected. The first pulse i0 is always set
 *    into the position corresponding to the global maximum value.
 *    Next, four iterations are carried out. During each iteration the position
 *    of pulse i1 is set to the local maximum of one track.
 *    The rest of the pulses are searched in pairs by sequentially searching
 *    each of the pulse pairs {i2,i3}, {i4,i5}, {i6,i7} and {i8,i9}
 *    in nested loops. Every pulse has 8 possible positions, i.e., there are
 *    four 8x8-loops, resulting in 256 different combinations of
 *    pulse positions for each iteration. In each iteration all the 9 pulse
 *    starting positions are cyclically shifted, so that the pulse pairs are
 *    changed and the pulse i1 is placed in a local maximum of
 *    a different track. The rest of the pulses are searched also for
 *    the other positions in the tracks. At least one pulse is located in
 *    a position corresponding to the global maximum and one pulse is
 *    located in a position corresponding to one of the 4 local maxima.
 *
 * Returns:
 *    void
 */
static void search_10i40( Float32 dn[], Float32 rr[][L_CODE], Word32 ipos[],
      Word32 pos_max[], Word32 codvec[] )
{
   Float32 rrv[L_CODE];
   Float32 psk, ps, ps0, ps1, ps2, sq, sq2, alpk, alp, alp0, alp1, alp2;
   Float32 *p_r, *p_r0, *p_r1, *p_r2, *p_r3, *p_r4, *p_r5, *p_r6, *p_r7, *p_r8,
         *p_r9, *p_r10;
   Float32 *p_rrv, *p_rrv0, *p_dn, *p_dn0, *p_dn1, *p_dn_max;
   Word32 i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, j, k, ia, ib, i, pos;

   p_dn_max = &dn[39];

   /* fix i0 on maximum of correlation position */
   i0 = pos_max[ipos[0]];
   ia = ib = 0;
   ps = 0;

   /* i1 loop */
   psk = -1;
   alpk = 1;

   for ( i = 0; i < 10; i++ ) {
      codvec[i] = i;
   }
   p_r = &rr[i0][i0];

   for ( i = 1; i < 5; i++ ) {
      i1 = pos_max[ipos[1]];
      i2 = ipos[2];
      i3 = ipos[3];
      i4 = ipos[4];
      i5 = ipos[5];
      i6 = ipos[6];
      i7 = ipos[7];
      i8 = ipos[8];
      i9 = ipos[9];
      ps0 = dn[i0] + dn[i1];
      alp0 = *p_r + rr[i1][i1] + 2.0F * rr[i0][i1];

      /* i2 and i3 loop	*/
      p_rrv = &rrv[i3];
      p_r0 = &rr[i0][i3];
      p_r1 = &rr[i1][i3];
      p_r3 = &rr[i3][i3];
      *p_rrv = *p_r3 + 2.0F * ( *p_r0 + *p_r1 );
      *( p_rrv + 5 ) = *( p_r3 + 205 ) + 2.0F * ( *( p_r0 + 5 ) + *( p_r1 + 5 )
            );
      *( p_rrv + 10 ) = *( p_r3 + 410 ) + 2.0F * ( *( p_r0 + 10 ) + *( p_r1 + 10
            ) );
      *( p_rrv + 15 ) = *( p_r3 + 615 ) + 2.0F * ( *( p_r0 + 15 ) + *( p_r1 + 15
            ) );
      *( p_rrv + 20 ) = *( p_r3 + 820 ) + 2.0F * ( *( p_r0 + 20 ) + *( p_r1 + 20
            ) );
      *( p_rrv + 25 ) = *( p_r3 + 1025 ) + 2.0F * ( *( p_r0 + 25 ) + *( p_r1 +
            25 ) );
      *( p_rrv + 30 ) = *( p_r3 + 1230 ) + 2.0F * ( *( p_r0 + 30 ) + *( p_r1 +
            30 ) );
      *( p_rrv + 35 ) = *( p_r3 + 1435 ) + 2.0F * ( *( p_r0 + 35 ) + *( p_r1 +
            35 ) );
      sq = -1;
      alp = 1;
      ps = 0;
      ia = i2;
      ib = i3;
      p_rrv = rrv + i3;
      p_r0 = &rr[i0][i2];
      p_r1 = &rr[i1][i2];
      p_r2 = &rr[i2][i2];
      p_r3 = &rr[i2][i3];
      p_dn0 = dn + i2;
      p_dn1 = dn + i3;
      p_rrv0 = rrv + i3;

      do {
         ps1 = ps0 + *p_dn0;
         alp1 = alp0 + *p_r2 + 2.0F * ( *p_r0 + *p_r1 );
         p_rrv = p_rrv0;
         p_dn = p_dn1;
         p_r4 = p_r3;

         do {
            ps2 = ps1 + *p_dn;
            sq2 = ps2 * ps2;
            alp2 = alp1 + *p_rrv + 2.0F * *p_r4;

            if ( ( alp * sq2 ) > ( sq * alp2 ) ) {
               sq = sq2;
               ps = ps2;
               alp = alp2;
               ia = ( Word16 )( p_dn0 - dn );
               ib = ( Word16 )( p_dn - dn );
            }
            p_rrv += 5;
            p_dn += 5;
            p_r4 += 5;
         } while ( p_dn < p_dn_max );
         p_dn0 += 5;
         p_r0 += 5;
         p_r1 += 5;
         p_r2 += 205;
         p_r3 += 200;
      } while ( p_dn0 <= p_dn_max );
      i2 = ia;
      i3 = ib;

      /* i4 and i5 loop	*/
      p_rrv = rrv + i5;
      p_r0 = &rr[i0][i5];
      p_r1 = &rr[i1][i5];
      p_r2 = &rr[i2][i5];
      p_r3 = &rr[i3][i5];
      p_r5 = &rr[i5][i5];
      *p_rrv = *p_r5 + 2.0F * ( *p_r0 + *p_r1 + *p_r2 + *p_r3 );
      *( p_rrv + 5 ) = *( p_r5 + 205 ) + 2.0F * ( *( p_r0 + 5 ) + *( p_r1 + 5 )
            + *( p_r2 + 5 ) + *( p_r3 + 5 ) );
      *( p_rrv + 10 ) = *( p_r5 + 410 ) + 2.0F * ( *( p_r0 + 10 ) + *( p_r1 + 10
            ) + *( p_r2 + 10 ) + *( p_r3 + 10 ) );
      *( p_rrv + 15 ) = *( p_r5 + 615 ) + 2.0F * ( *( p_r0 + 15 ) + *( p_r1 + 15
            ) + *( p_r2 + 15 ) + *( p_r3 + 15 ) );
      *( p_rrv + 20 ) = *( p_r5 + 820 ) + 2.0F * ( *( p_r0 + 20 ) + *( p_r1 + 20
            ) + *( p_r2 + 20 ) + *( p_r3 + 20 ) );
      *( p_rrv + 25 ) = *( p_r5 + 1025 ) + 2.0F * ( *( p_r0 + 25 ) + *( p_r1 +
            25 ) + *( p_r2 + 25 ) + *( p_r3 + 25 ) );
      *( p_rrv + 30 ) = *( p_r5 + 1230 ) + 2.0F * ( *( p_r0 + 30 ) + *( p_r1 +
            30 ) + *( p_r2 + 30 ) + *( p_r3 + 30 ) );
      *( p_rrv + 35 ) = *( p_r5 + 1435 ) + 2.0F * ( *( p_r0 + 35 ) + *( p_r1 +
            35 ) + *( p_r2 + 35 ) + *( p_r3 + 35 ) );

      /* Default value */
      ps0 = ps;
      alp0 = alp;
      sq = -1;
      alp = 1;
      ps = 0;
      ia = i4;
      ib = i5;
      p_dn0 = dn + i4;
      p_dn1 = dn + i5;
      p_r0 = &rr[i0][i4];
      p_r1 = &rr[i1][i4];
      p_r2 = &rr[i2][i4];
      p_r3 = &rr[i3][i4];
      p_r4 = &rr[i4][i4];
      p_r5 = &rr[i4][i5];
      p_rrv0 = rrv + i5;

      do {
         ps1 = ps0 + *p_dn0;
         alp1 = alp0 + *p_r4 + 2.0F * ( *p_r0 + *p_r1 + *p_r2 + *p_r3 );
         p_dn = p_dn1;
         p_r6 = p_r5;
         p_rrv = p_rrv0;

         do {
            ps2 = ps1 + *p_dn;
            sq2 = ps2 * ps2;
            alp2 = alp1 + *p_rrv + 2.0F * *p_r6;

            if ( ( alp * sq2 ) > ( sq * alp2 ) ) {
               sq = sq2;
               ps = ps2;
               alp = alp2;
               ia = ( Word16 )( p_dn0 - dn );
               ib = ( Word16 )( p_dn - dn );
            }
            p_dn += 5;
            p_rrv += 5;
            p_r6 += 5;
         } while ( p_dn <= p_dn_max );
         p_r0 += 5;
         p_r1 += 5;
         p_r2 += 5;
         p_r3 += 5;
         p_r4 += 205;
         p_r5 += 200;
         p_dn0 += 5;
      } while ( p_dn0 < p_dn_max );
      i4 = ia;
      i5 = ib;

      /* i6 and i7 loop	*/
      p_rrv = rrv + i7;
      p_r0 = &rr[i0][i7];
      p_r1 = &rr[i1][i7];
      p_r2 = &rr[i2][i7];
      p_r3 = &rr[i3][i7];
      p_r4 = &rr[i4][i7];
      p_r5 = &rr[i5][i7];
      p_r7 = &rr[i7][i7];
      *p_rrv = *p_r7 + 2.0F * ( *p_r0 + *p_r1 + *p_r2 + *p_r3 + *p_r4 + *p_r5 );
      *( p_rrv + 5 ) = *( p_r7 + 205 ) + 2.0F * ( *( p_r0 + 5 ) + *( p_r1 + 5 )
            + *( p_r2 + 5 ) + *( p_r3 + 5 ) + *( p_r4 + 5 ) + *( p_r5 + 5 ) );
      *( p_rrv + 10 ) = *( p_r7 + 410 ) + 2.0F * ( *( p_r0 + 10 ) + *( p_r1 + 10
            ) + *( p_r2 + 10 ) + *( p_r3 + 10 ) + *( p_r4 + 10 ) + *( p_r5 + 10
            ) );
      *( p_rrv + 15 ) = *( p_r7 + 615 ) + 2.0F * ( *( p_r0 + 15 ) + *( p_r1 + 15
            ) + *( p_r2 + 15 ) + *( p_r3 + 15 ) + *( p_r4 + 15 ) + *( p_r5 + 15
            ) );
      *( p_rrv + 20 ) = *( p_r7 + 820 ) + 2.0F * ( *( p_r0 + 20 ) + *( p_r1 + 20
            ) + *( p_r2 + 20 ) + *( p_r3 + 20 ) + *( p_r4 + 20 ) + *( p_r5 + 20
            ) );
      *( p_rrv + 25 ) = *( p_r7 + 1025 ) + 2.0F * ( *( p_r0 + 25 ) + *( p_r1 +
            25 ) + *( p_r2 + 25 ) + *( p_r3 + 25 ) + *( p_r4 + 25 ) + *( p_r5 +
            25 ) );
      *( p_rrv + 30 ) = *( p_r7 + 1230 ) + 2.0F * ( *( p_r0 + 30 ) + *( p_r1 +
            30 ) + *( p_r2 + 30 ) + *( p_r3 + 30 ) + *( p_r4 + 30 ) + *( p_r5 +
            30 ) );
      *( p_rrv + 35 ) = *( p_r7 + 1435 ) + 2.0F * ( *( p_r0 + 35 ) + *( p_r1 +
            35 ) + *( p_r2 + 35 ) + *( p_r3 + 35 ) + *( p_r4 + 35 ) + *( p_r5 +
            35 ) );

      /* Default value */
      ps0 = ps;
      alp0 = alp;
      sq = -1;
      alp = 1;
      ps = 0;
      ia = i6;
      ib = i7;
      p_dn0 = dn + i6;
      p_dn1 = dn + i7;
      p_r0 = &rr[i0][i6];
      p_r1 = &rr[i1][i6];
      p_r2 = &rr[i2][i6];
      p_r3 = &rr[i3][i6];
      p_r4 = &rr[i4][i6];
      p_r5 = &rr[i5][i6];
      p_r6 = &rr[i6][i6];
      p_r7 = &rr[i6][i7];
      p_rrv0 = rrv + i7;

      do {
         ps1 = ps0 + *p_dn0;
         alp1 = alp0 + *p_r6 + 2.0F * ( *p_r0 + *p_r1 + *p_r2 + *p_r3 + *p_r4 +
               *p_r5 );
         p_dn = p_dn1;
         p_r8 = p_r7;
         p_rrv = p_rrv0;

         do {
            ps2 = ps1 + *p_dn;
            sq2 = ps2 * ps2;
            alp2 = alp1 + *p_rrv + 2.0F * *p_r8;

            if ( ( alp * sq2 ) > ( sq * alp2 ) ) {
               sq = sq2;
               ps = ps2;
               alp = alp2;
               ia = ( Word16 )( p_dn0 - dn );
               ib = ( Word16 )( p_dn - dn );
            }
            p_dn += 5;
            p_rrv += 5;
            p_r8 += 5;
         } while ( p_dn <= p_dn_max );
         p_r0 += 5;
         p_r1 += 5;
         p_r2 += 5;
         p_r3 += 5;
         p_r4 += 5;
         p_r5 += 5;
         p_r6 += 205;
         p_r7 += 200;
         p_dn0 += 5;
      } while ( p_dn0 < p_dn_max );
      i6 = ia;
      i7 = ib;

      /* i8 and i9 loop	*/
      p_rrv = rrv + i9;
      p_r0 = &rr[i0][i9];
      p_r1 = &rr[i1][i9];
      p_r2 = &rr[i2][i9];
      p_r3 = &rr[i3][i9];
      p_r4 = &rr[i4][i9];
      p_r5 = &rr[i5][i9];
      p_r6 = &rr[i6][i9];
      p_r7 = &rr[i7][i9];
      p_r9 = &rr[i9][i9];
      *p_rrv = *p_r9 + 2.0F * ( *p_r0 + *p_r1 + *p_r2 + *p_r3 + *p_r4 + *p_r5 +
            *p_r6 + *p_r7 );
      *( p_rrv + 5 ) = *( p_r9 + 205 ) + 2.0F * ( *( p_r0 + 5 ) + *( p_r1 + 5 )
            + *( p_r2 + 5 ) + *( p_r3 + 5 ) + *( p_r4 + 5 ) + *( p_r5 + 5 ) + *(
            p_r6 + 5 ) + *( p_r7 + 5 ) );
      *( p_rrv + 10 ) = *( p_r9 + 410 ) + 2.0F * ( *( p_r0 + 10 ) + *( p_r1 + 10
            ) + *( p_r2 + 10 ) + *( p_r3 + 10 ) + *( p_r4 + 10 ) + *( p_r5 + 10
            ) + *( p_r6 + 10 ) + *( p_r7 + 10 ) );
      *( p_rrv + 15 ) = *( p_r9 + 615 ) + 2.0F * ( *( p_r0 + 15 ) + *( p_r1 + 15
            ) + *( p_r2 + 15 ) + *( p_r3 + 15 ) + *( p_r4 + 15 ) + *( p_r5 + 15
            ) + *( p_r6 + 15 ) + *( p_r7 + 15 ) );
      *( p_rrv + 20 ) = *( p_r9 + 820 ) + 2.0F * ( *( p_r0 + 20 ) + *( p_r1 + 20
            ) + *( p_r2 + 20 ) + *( p_r3 + 20 ) + *( p_r4 + 20 ) + *( p_r5 + 20
            ) + *( p_r6 + 20 ) + *( p_r7 + 20 ) );
      *( p_rrv + 25 ) = *( p_r9 + 1025 ) + 2.0F * ( *( p_r0 + 25 ) + *( p_r1 +
            25 ) + *( p_r2 + 25 ) + *( p_r3 + 25 ) + *( p_r4 + 25 ) + *( p_r5 +
            25 ) + *( p_r6 + 25 ) + *( p_r7 + 25 ) );
      *( p_rrv + 30 ) = *( p_r9 + 1230 ) + 2.0F * ( *( p_r0 + 30 ) + *( p_r1 +
            30 ) + *( p_r2 + 30 ) + *( p_r3 + 30 ) + *( p_r4 + 30 ) + *( p_r5 +
            30 ) + *( p_r6 + 30 ) + *( p_r7 + 30 ) );
      *( p_rrv + 35 ) = *( p_r9 + 1435 ) + 2.0F * ( *( p_r0 + 35 ) + *( p_r1 +
            35 ) + *( p_r2 + 35 ) + *( p_r3 + 35 ) + *( p_r4 + 35 ) + *( p_r5 +
            35 ) + *( p_r6 + 35 ) + *( p_r7 + 35 ) );

      /* Default value */
      ps0 = ps;
      alp0 = alp;
      sq = -1;
      alp = 1;
      ps = 0;
      ia = i8;
      ib = i9;
      p_dn0 = dn + i8;
      p_dn1 = dn + i9;
      p_r0 = &rr[i0][i8];
      p_r1 = &rr[i1][i8];
      p_r2 = &rr[i2][i8];
      p_r3 = &rr[i3][i8];
      p_r4 = &rr[i4][i8];
      p_r5 = &rr[i5][i8];
      p_r6 = &rr[i6][i8];
      p_r7 = &rr[i7][i8];
      p_r8 = &rr[i8][i8];
      p_r9 = &rr[i8][i9];
      p_rrv0 = rrv + i9;

      do {
         ps1 = ps0 + *p_dn0;
         alp1 = alp0 + *p_r8 + 2.0F * ( *p_r0 + *p_r1 + *p_r2 + *p_r3 + *p_r4 +
               *p_r5 + *p_r6 + *p_r7 );
         p_dn = p_dn1;
         p_r10 = p_r9;
         p_rrv = p_rrv0;

         do {
            ps2 = ps1 + *p_dn;
            sq2 = ps2 * ps2;
            alp2 = alp1 + *p_rrv + 2.0F * *p_r10;

            if ( ( alp * sq2 ) > ( sq * alp2 ) ) {
               sq = sq2;
               ps = ps2;
               alp = alp2;
               ia = ( Word16 )( p_dn0 - dn );
               ib = ( Word16 )( p_dn - dn );
            }
            p_dn += 5;
            p_rrv += 5;
            p_r10 += 5;
         } while ( p_dn <= p_dn_max );
         p_r0 += 5;
         p_r1 += 5;
         p_r2 += 5;
         p_r3 += 5;
         p_r4 += 5;
         p_r5 += 5;
         p_r6 += 5;
         p_r7 += 5;
         p_r8 += 205;
         p_r9 += 200;
         p_dn0 += 5;
      } while ( p_dn0 < p_dn_max );

      /*
       * test and memorise if this combination is better than the last one.
       */
      if ( ( alpk * sq ) > ( psk * alp ) ) {
         psk = sq;
         alpk = alp;
         codvec[0] = ( Word16 )i0;
         codvec[1] = ( Word16 )i1;
         codvec[2] = ( Word16 )i2;
         codvec[3] = ( Word16 )i3;
         codvec[4] = ( Word16 )i4;
         codvec[5] = ( Word16 )i5;
         codvec[6] = ( Word16 )i6;
         codvec[7] = ( Word16 )i7;
         codvec[8] = ( Word16 )ia;
         codvec[9] = ( Word16 )ib;
      }

      /*
       * Cyclic permutation of i1,i2,i3,i4,i5,i6,i7,(i8 and i9).
       */
      pos = ipos[1];

      for ( j = 1, k = 2; k < 10; j++, k++ ) {
         ipos[j] = ipos[k];
      }
      ipos[9] = pos;
   }   /* end 1..nb_tracks  loop*/
}


/*
 * build_code_10i40_35bits
 *
 *
 * Parameters:
 *    codvec            I: position of pulses
 *    dn_sign           I: sign of pulses
 *    cod               O: algebraic codebook vector
 *    h                 I: impulse response of weighted synthesis filter
 *    y                 O: filtered innovative code
 *    indx              O: index of 10 pulses (sign+position)
 *
 * Function:
 *    Builds the codeword, the filtered codeword and index of the
 *    codevector, based on the signs and positions of 10 pulses.
 *
 * Returns:
 *    void
 */
static void build_code_10i40_35bits( Word32 codvec[], Float32 dn_sign[], Float32
      cod[], Float32 h[], Float32 y[], Word16 indx[] )
{
   Word32 i, j, k, track, index, sign[10];
   Float32 *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;
   Float64 s;


   memset( cod, 0, 160 );
   memset( y, 0, 160 );

   for ( i = 0; i < NB_TRACK; i++ ) {
      indx[i] = -1;
   }

   for ( k = 0; k < 10; k++ ) {
      /* read pulse position */
      i = codvec[k];

      /* read sign */
      j = ( Word16 )dn_sign[i];

      /* index = pos/5 */
      index = ( Word16 )( i / 5 );

      /* track = pos%5 */
      track = ( Word16 )( i % 5 );

      if ( j > 0 ) {
         cod[i] = cod[i] + 1;
         sign[k] = 1;
      }
      else {
         cod[i] = cod[i] - 1;
         sign[k] = -1;
         index = index + 8;
      }

      if ( indx[track] < 0 ) {
         indx[track] = ( Word16 )index;
      }
      else {
         if ( ( ( index ^ indx[track] ) & 8 ) == 0 ) {
            /* sign of 1st pulse == sign of 2nd pulse */
            if ( indx[track] <= index ) {
               indx[track + 5] = ( Word16 )index;
            }
            else {
               indx[track + 5] = ( Word16 )indx[track];
               indx[track] = ( Word16 )index;
            }
         }
         else {
            /* sign of 1st pulse != sign of 2nd pulse */
            if ( ( indx[track] & 7 ) <= ( index & 7 ) ) {
               indx[track + 5] = ( Word16 )indx[track];
               indx[track] = ( Word16 )index;
            }
            else {
               indx[track + 5] = ( Word16 )index;
            }
         }
      }
   }
   p0 = h - codvec[0];
   p1 = h - codvec[1];
   p2 = h - codvec[2];
   p3 = h - codvec[3];
   p4 = h - codvec[4];
   p5 = h - codvec[5];
   p6 = h - codvec[6];
   p7 = h - codvec[7];
   p8 = h - codvec[8];
   p9 = h - codvec[9];

   for ( i = 0; i < L_CODE; i++ ) {
      s = *p0++ * sign[0];
      s += *p1++ * sign[1];
      s += *p2++ * sign[2];
      s += *p3++ * sign[3];
      s += *p4++ * sign[4];
      s += *p5++ * sign[5];
      s += *p6++ * sign[6];
      s += *p7++ * sign[7];
      s += *p8++ * sign[8];
      s += *p9++ * sign[9];
      y[i] = ( Float32 )( s );
   }
}


/*
 * q_p
 *
 *
 * Parameters:
 *    ind               B: Pulse position
 *    n                 I: Pulse number
 *
 * Function:
 *    Gray coding
 *
 * Returns:
 *    void
 */
static void q_p( Word16 *ind, Word32 n )
{
   Word16 tmp;


   tmp = *ind;

   if ( n < 5 ) {
      *ind = ( Word16 )( ( tmp & 0x8 ) | gray[tmp & 0x7] );
   }
   else {
      *ind = gray[tmp & 0x7];
   }
}


/*
 * code_10i40_35bits
 *
 *
 * Parameters:
 *    x                 I: target vector
 *    cn                I: residual after long term prediction
 *    h                 I: impulse response of weighted synthesis filter
 *    gain_pit          I: quantified adaptive codebook gain
 *    code              O: algebraic (fixed) codebook excitation
 *    y                 O: filtered fixed codebook excitation
 *    anap              O: 7 Word16, index of 8 pulses (signs+positions)
 *
 * Function:
 *    Searches a 35 bit algebraic codebook containing 10 pulses
 *    in a frame of 40 samples.
 *
 *    The code contains 10 nonzero pulses: i0...i9.
 *    All pulses can have two possible amplitudes: +1 or -1.
 *    The 40 positions in a subframe are divided into 5 tracks of
 *    interleaved positions. Each track contains two pulses.
 *    The pulses can have the following possible positions:
 *       Track    Pulse       Positions
 *          1     i0, i5      0, 5, 10, 15, 20, 25, 30, 35.
 *          2     i1, i6      1, 6, 11, 16, 21, 26, 31, 36.
 *          3     i2, i       2, 7, 12, 17, 22, 27, 32, 37.
 *          4     i3, i8      3, 8, 13, 18, 23, 28, 33, 38.
 *          5     i4, i9      4, 9, 14, 19, 24, 29, 34, 39.
 *
 *    Each pair of pulses require 1 bit for their signs and 6 bits for their
 *    positions (3 bits + 3 bits). This results in a 35 bit codebook.
 *    The function determines the optimal pulse signs and positions, builds
 *    the codevector, and computes the filtered codevector.
 *
 *    The algebraic codebook is searched by minimizing the mean square error
 *    between the weighted input speech and the weighted synthesized speech.
 *    The target signal used in the closed-loop pitch search is updated by
 *    subtracting the adaptive codebook contribution. That is:
 *
 *    x2(n) = x(n) - Gp' * y(n), n = 0, ..., 39
 *
 *    where y(n) = v(n) * h(n) is the filtered adaptive codebook vector
 *    and Gp' is the quantified adaptive codebook gain. This is done
 *    already in function cl_ltp.
 *
 *    If c(k) is the algebraic codevector at index k, then
 *    the algebraic codebook is searched by maximizing the term:
 *
 *    A(k) = (C(k) * C(k)) / Ed(k) =
 *
 *                     39
 *                   [SUM sb(i)*d(i)]^2
 *                    i=0
 *                  ---------------------
 *                 transpose(sb) * PI * sb
 *
 *    where d(n) is the correlation between the target signal x2(n)
 *    and the impulse response h(n), H is a the lower triangular Toepliz
 *    convolution matrix with diagonal h(0) and lower diagonals
 *    h(1), ..., h(39), and PI = H_transpose * H is the matrix of
 *    correlations of h(n).
 *
 *    The pulse amplitudes are preset by the mere quantization of an signal
 *    b(n) used for presetting the amplitudes.
 *
 *                           39
 *    b(n) = res(n) / SQRT[ SUM[ res(i) * res(i) ] ]
 *                          i=0
 *                          39
 *          + d(n) / SQRT[ SUM [ d(i) * d(i) ] ], n = 0, ..., 39,
 *                         i=0
 *
 *    where res(n) is normalized long-term prediction residual and
 *    d(n) is normalized vector.
 *
 *            39
 *    d(n) = SUM[ x2(i) * h(i-n) ], n = 0, ..., 39,
 *           i=n
 *
 *    This is simply done by setting the amplitude of a pulse at
 *    a certain position equal to the sign of b(n) at that position.
 *    The simplification proceeds as follows (prior to the codebook search).
 *    First, the sign signal sb(n) = SIGN[ b(n) ] and
 *    the signal d'(n) = sb(n) * d(n) are computed.
 *
 * Returns:
 *    void
 */
static void code_10i40_35bits( Float32 x[], Float32 cn[], Float32 h[],
    Word32 T0, Float32 gain_pit, Float32 code[],
    Float32 y[], Word16 anap[] )
 {
    Float32 rr[L_CODE][L_CODE];
    Float32 dn[L_CODE], sign[L_CODE];
    Word32 ipos[10], pos_max[NB_TRACK], codvec[10];
    Word32 i;

    /* include pitch contribution into impulse resp. */
    if ( gain_pit > 1.0F )
       gain_pit = 1.0F;

    if ( gain_pit != 0 ) {
       for ( i = T0; i < L_SUBFR; i++ ) {
          h[i] += h[i - T0] * gain_pit;
       }
    }
    /*
    *            39
    *    d(n) = SUM[ x2(i) * h(i-n) ], n = 0, ..., 39
    *           i=n
    */
    cor_h_x( h, x, dn );

    /* sb(n) and d'(n) */
    set_sign12k2( dn, cn, sign, pos_max, NB_TRACK, ipos, STEP );

    /* Matrix of correlations */
    cor_h( h, sign, rr );
    search_10i40( dn, rr, ipos, pos_max, codvec );
    build_code_10i40_35bits( codvec, sign, code, h, y, anap );

    for ( i = 0; i < 10; i++ ) {
       q_p( &anap[i], i );
    }

    /*  Add the pitch contribution to code[]. */
    if ( gain_pit != 0 ) {
       for ( i = T0; i < L_SUBFR; i++ ) {
          code[i] += code[i - T0] * gain_pit;
       }
    }
    return;
 }


/*
 * cbsearch
 *
 *
 * Parameters:
 *    mode              I: AMR mode
 *    subnr             I: Subframe
 *    x                 I: Target vector
 *    h                 B: Impulse response of weighted synthesis filter
 *    T0                I: Pitch lag
 *    pitch_sharp       I: Last quantized pitch gain
 *    gain_pit          I: Algebraic codebook gain
 *    code              O: Innovative codebook
 *    y                 O: Filtered fixed codebook excitation
 *    res2              I: residual after long term prediction
 *    anap              O: Signs and positions of the pulses
 *
 * Function:
 *    Innovative codebook search (find index and gain)
 *
 * Returns:
 *    void
 */
static void cbsearch( enum Mode mode, Word16 subnr, Float32 x[],
                     Float32 h[], Word32 T0, Float32 pitch_sharp,
                     Float32 gain_pit, Float32 code[], Float32 y[],
                     Float32 *res2, Word16 **anap )
{
   switch (mode){
   default:
      code_10i40_35bits( x, res2, h, T0, gain_pit, code, y, *anap );
      *anap += 10;
   }
}

/*
 * Log2_norm
 *
 *
 * Parameters:
 *    x                 I: input value
 *    exp               I: exponent
 *    exponent          O: Integer part of Log2. (range: 0<=val<=30)
 *    fraction          O: Fractional part of Log2. (range: 0<=val<1)
 *
 * Function:
 *    Computes log2
 *
 *    Computes log2(L_x, exp),  where   L_x is positive and
 *    normalized, and exp is the normalisation exponent
 *    If L_x is negative or zero, the result is 0.
 *
 *    The function Log2(L_x) is approximated by a table and linear
 *    interpolation. The following steps are used to compute Log2(L_x)
 *
 *    exponent = 30-normExponent
 *    i = bit25-b31 of L_x;  32<=i<=63  (because of normalization).
 *    a = bit10-b24
 *    i -=32
 *    fraction = table[i]<<16 - (table[i] - table[i+1]) * a * 2
 *
 * Returns:
 *    void
 */
static void Log2_norm( Word32 x, Word32 exp, Word32 *exponent, Word32 *
      fraction )
{
   Word32 y, i, a;

   if ( x <= 0 ) {
      *exponent = 0;
      *fraction = 0;
      return;
   }

   /* Extract b25-b31 */
   i = x >> 25;
   i = i - 32;

   /* Extract b10-b24 of fraction */
   a = x >> 9;
   a = a & 0xFFFE;   /* 2a */

   /* fraction */
   y = ( log2_table[i] << 16 ) - a * ( log2_table[i] - log2_table[i + 1] );
   *fraction = y >> 16;
   *exponent = 30 - exp;
   return;
}

/*
 * Log2
 *
 *
 * Parameters:
 *    x                 I: input value
 *    exponent          O: Integer part of Log2. (range: 0<=val<=30)
 *    fraction          O: Fractional part of Log2. (range: 0<=val<1)
 *
 * Function:
 *    Computes log2(L_x)
 *    If x is negative or zero, the result is 0.
 *
 * Returns:
 *    void
 */
static void Log2( Word32 x, Word32 *exponent, Word32 *fraction )
{
   int exp;


   frexp( ( Float64 )x, &exp );
   exp = 31 - exp;
   Log2_norm( x <<exp, exp, exponent, fraction );
}

/*
 * Pow2
 *
 *
 * Parameters:
 *    exponent          I: Integer part. (range: 0<=val<=30)
 *    fraction          O: Fractional part. (range: 0.0<=val<1.0)
 *
 * Function:
 *    pow(2.0, exponent.fraction)
 *
 *    The function Pow2(L_x) is approximated by a table and linear interpolation.
 *
 *    i = bit10-b15 of fraction, 0 <= i <= 31
 *    a = biT0-b9   of fraction
 *    x = table[i]<<16 - (table[i] - table[i+1]) * a * 2
 *    x = L_x >> (30-exponent) (with rounding)
 *
 * Returns:
 *    result (range: 0<=val<=0x7fffffff)
 */
static Word32 Pow2( Word32 exponent, Word32 fraction )
{
   Word32 i, a, tmp, x, exp;

   /* Extract b10-b16 of fraction */
   i = fraction >> 10;

   /* Extract b0-b9 of fraction */
   a = ( fraction << 5 ) & 0x7fff;

   /* table[i] << 16 */
   x = pow2_table[i] << 16;

   /* table[i] - table[i+1] */
   tmp = pow2_table[i] - pow2_table[i + 1];

   /* L_x -= tmp*a*2 */
   x -= ( tmp * a ) << 1;

   if ( exponent >= -1 ) {
      exp = ( 30 - exponent );

      /* Rounding */
      if ( ( x & ( ( Word32 )1 << ( exp - 1 ) ) ) != 0 ) {
         x = ( x >> exp ) + 1;
      }
      else
         x = x >> exp;
   }
   else
      x = 0;
   return( x );
}

/*
 * gc_pred
 *
 *
 * Parameters:
 *    past_qua_en       I: MA predictor
 *    mode              I: AMR mode
 *    code              I: innovative codebook vector
 *    gcode0            O: predicted gain factor
 *    en                I: innovation energy (only calculated for MR795)
 *
 * Function:
 *    MA prediction of the innovation energy
 *
 *    Mean removed innovation energy (dB) in subframe n
 *                          N-1
 *    E(n) = 10*log(gc*gc * SUM[(code(i) * code(i)]/N) - E_mean
 *                          i=0
 *    N=40
 *
 *    Mean innovation energy (dB)
 *                   N-1
 *    Ei(n) = 10*log(SUM[(code(i) * code(i)]/N)
 *                   i=0
 *
 *    Predicted energy
 *             4
 *    Ep(n) = SUM[b(i) * R(n-i)]
 *            i=1
 *    b = [0.68 0.58 0.34 0.19]
 *    R(k) is quantified prediction error at subframe k
 *
 *    E_Mean = 36 dB (MR122)
 *
 *    Predicted gain gc is found by
 *
 *    gc = POW[10, 0.05 * (Ep(n) + E_mean - Ei)]
 *
 * Returns:
 *    void
 */
static void gc_pred( Word32 *past_qua_en, enum Mode mode, Float32 *code,
      Word32 *gcode0_exp, Word32 *gcode0_fra, Float32 *en )
{
   Float64 ener_code;
   Word32 exp, frac, ener, ener_tmp;


   /* energy of code */
   ener_code = Dotproduct40( code, code );

   if ( mode == MR122 ) {

      ener = (Word32)(ener_code * 33554432);
      /* ener_code = ener_code / lcode; lcode = 40; 1/40 = 26214 Q20       */
      ener = ( ( ener + 0x00008000L ) >> 16 ) * 52428;

      Log2( ener, &exp, &frac );
      ener = ( ( exp - 30 ) << 16 ) + ( frac << 1 );

      ener_tmp = 44 * qua_gain_code_MR122[past_qua_en[0]];
      ener_tmp += 37 * qua_gain_code_MR122[past_qua_en[1]];
      ener_tmp += 22 * qua_gain_code_MR122[past_qua_en[2]];
      ener_tmp += 12 * qua_gain_code_MR122[past_qua_en[3]];

      ener_tmp = ener_tmp << 1;
      ener_tmp += 783741L;

      /*
       * predicted codebook gain
       * gc0 = Pow10( (ener*constant - ener_code*constant) / 20 )
       *     = Pow2(ener-ener_code)
       *     = Pow2(int(d)+frac(d))
       */
      ener = ( ener_tmp - ener ) >> 1;   /* Q16 */
      *gcode0_exp = ener >> 16;
      *gcode0_fra = ( ener >> 1 ) - ( *gcode0_exp << 15 );
   }
 }


/*
 * q_gain_code
 *
 *
 * Parameters:
 *    gcode0            I: predicted CB gain
 *    gain              B: quantized fixed codebook gain
 *    qua_ener_index    O: quantized energy error index
 *
 * Function:
 *    Scalar quantization of the innovative codebook gain.
 *
 *    A correction factor between the gain gc and the estimated one gc'
 *    is given by:
 *
 *    cf = gc/gc'
 *
 *    The quantization table search is performed by minimizing the error:
 *
 *    Eq = POW[gc - cf' * gc', 2]
 *
 *    Once the optimum value cf' is chosen,
 *    the quantified fixed codebook gain is given by
 *
 *    gc_q = cf' * gc'
 *
 * Returns:
 *    index             quantization index
 */
static Word16 q_gain_code( Float32 gcode0, Float32 *gain, Word32 *qua_ener_index)
{
   Float64 err_min, err;
   const Float32 *p;
   Word32 i, index;


   p = &gain_factor[0];

   /* using abs instead pow */
   err_min = fabs( *gain - ( gcode0 * *p++ ) );
   index = 0;

   for ( i = 1; i < NB_QUA_CODE; i++ ) {
      err = fabs( *gain - ( gcode0 * *p++ ) );

      if ( err < err_min ) {
         err_min = err;
         index = i;
      }
   }
   p = &gain_factor[index];
   *gain = (Float32)floor(gcode0 * *p);
   *qua_ener_index = index;

   return( Word16 )index;
}


/*
 * gainQuant
 *
 *
 * Parameters:
 *    gcode0            I: predicted CB gain
 *    coeff             I: energy coefficients (5)
 *    gp_limit          I: pitch gain limit
 *    gain_pit          O: Pitch gain
 *    gain_cod          O: Code gain
 *    qua_ener          O: quantized energy error,
 *    mode              I: AMR mode
 *    even_subframe     I: even subframe indicator flag
 *    past_qua_en       B: past quantized energies [4]
 *    past_qua_en_unq   B: past energies [4]
 *    sf0_coeff         B: energy coefficients subframe 0 (or 2)
 *    sf0_target_en     B: target energy from subframe 0 (or 2)
 *    sf0_gcode0        B: predicted gain factor subframe 0 (or 2)
 *    gain_idx_ptr      B: gain index pointer
 *    sf0_gain_pit      B: Pitch gain subframe 0 (or 2)
 *    sf0_gain_cod      B: Code gain subframe 0 (or 2)
 *    res               I: LP residual
 *    exc               I: LTP excitation (unfiltered)
 *    code              I: innovative codebook vector
 *    xn                I: Target vector
 *    xn2               I: CB target vector
 *    y1                I: Adaptive codebook
 *    y2                I: Filtered innovative vector
 *    gCoeff            I: Correlations <xn y1> <y1 y1>
 *    gp_limit          I: pitch gain limit
 *    gain_pit          O: Pitch gain
 *    gain_cod          O: Code gain
 *    prev_gc           B: B: previous code gain
 *    onset             B: onset indicator
 *    ltpg_mem          B: stored past LTP coding gains
 *    prev_alpha        B: previous gain adaptation factor
 *    anap              B: Index of quantization
 *
 * Function:
 *    Quantization of gains
 *
 * Returns:
 *    index             index of quantization
 */
static void gainQuant( enum Mode mode, Word32 even_subframe, Word32 *
      past_qua_en, Word32 *past_qua_en_unq, Float32 *sf0_coeff, Float32 *
      sf0_target_en, Word32 *sf0_gcode0_exp, Word32 *sf0_gcode0_fra,Word16 **gain_idx_ptr, Float32 *
      sf0_gain_pit, Float32 *sf0_gain_cod, Float32 *res, Float32 *exc, Float32
      code[], Float32 xn[], Float32 xn2[], Float32 y1[], Float32 y2[], Float32
      gCoeff[], Float32 gp_limit, Float32 *gain_pit, Float32 *gain_cod, Float32
      *prev_gc, Word16 *onset, Float32 *ltpg_mem, Float32 *prev_alpha, Word16 **
      anap )
{
   Float32 gcode0, en = 0;
   Word32 i, exp, frac, qua_ener_index;

   {
      /*
       * predict codebook gain and quantize
       *  (also compute normalized CB innovation energy for MR795)
       */
      gc_pred( past_qua_en, mode, code, &exp, &frac, &en );

      if ( mode == MR122 ) {
         /*
          * Compute the innovative codebook gain.
          * The innovative codebook gain is given by
          * g = <xn2[], y2[]> / <y2[], y2[]>
          * where xn2[] is the target vector,
          * y2[] is the filtered innovative
          * codevector
          */

         gcode0 = (Float32)Pow2( exp, frac );
         /* saturation at decoder */
         if (gcode0 > 2047.9375F) gcode0 = 2047.9375F;

         *gain_cod = (Float32)(Dotproduct40( xn2, y2 ) / ( Dotproduct40( y2, y2 )+ 0.01F ));

         if ( *gain_cod < 0 )
            *gain_cod = 0.0F;
         *( *anap )++ = q_gain_code( gcode0, gain_cod,&qua_ener_index);
      }
   

      /*
       * update table of past quantized energies
       */
      for ( i = 3; i > 0; i-- ) {
         past_qua_en[i] = past_qua_en[i - 1];
      }
      past_qua_en[0] = qua_ener_index;

   }
}


/*
 * subframePostProc
 *
 *
 * Parameters:
 *    speech            I: Speech segment
 *    i_subfr           I: Subframe number
 *    gain_pit          I: Pitch gain
 *    gain_code         I: Decoded innovation gain
 *    a_q               I: A(z) quantized for the 4 subframes
 *    synth             I: Local synthesis
 *    xn                I: Target vector for pitch search
 *    code              I: Fixed codebook exitation
 *    y1                I: Filtered adaptive exitation
 *    y2                I: Filtered fixed codebook excitation
 *    mem_syn           B: memory of synthesis filter
 *    mem_err           O: pointer to error signal
 *    mem_w0            O: memory of weighting filter
 *    exc               O: long term prediction residual
 *    sharp             O: pitch sharpening value
 *
 * Function:
 *    Subframe post processing
 *
 *    Memory update (all modes)
 *    An update of the states of the synthesis and weighting filters is needed
 *   in order to compute the target signal in the next subframe.
 *   After the two gains are quantified, the excitation signal, u(n),
 *   in the present subframe is found by:
 *
 *   u(n) = Gp_q * v(n) + Gc_q * c(n), n = 0, ..., 39,
 *
 *   where Gp_q and Gc_q are the quantified adaptive and fixed codebook gains,
 *   respectively, v(n) the adaptive codebook vector
 *   (interpolated past excitation), and c(n) is the fixed codebook vector
 *   (algebraic code including pitch sharpening). The states of the filters
 *   can be updated by filtering the signal res_lp(n) - u(n)
 *   (difference between residual and excitation) through the filters
 *   1 / A_q(z) and A(z/g1) / A(z/g2) for the 40-sample subframe and saving
 *   the states of the filters. This would require 3 filterings.
 *   A simpler approach which requires only one filtering is as follows.
 *   The local synthesized speech, S_s(n), is computed by filtering
 *   the excitation signal through 1 / A_q(z). The output of the filter
 *   due to the input res_lp(n) - u(n) is equivalent to e(n) = S(n) - S_s(n).
 *   So the states of the synthesis filter 1 / A_q(z) are given by
 *   e(n), n = 30, ..., 39. Updating the states of the filter A(z/g1) / A(z/g2)
 *   can be done by filtering the error signal e(n) through this filter
 *   to find the perceptually weighted error ew(n). However, the signal ew(n)
 *   can be equivalently found by:
 *
 *   ew(n) = x(n) - Gp_q * y(n) - Gc_q(n) * z(n)
 *
 *   Since the signals x(n), y(n) and z(n) are available, the states of
 *   the weighting filter are updated by computing ew(n) for n = 30, ..., 39.
 *   This saves two filterings.
 *
 * Returns:
 *    void
 */
static void subframePostProc( Float32 *speech, Word16 i_subfr, Float32 gain_pit,
      Float32 gain_code, Float32 *a_q, Float32 synth[], Float32 xn[], Float32
      code[], Float32 y1[], Float32 y2[], Float32 *mem_syn, Float32 *mem_err,
      Float32 *mem_w0, Float32 *exc, Float32 *sharp )
{
   Word32 i, j;


   /*
    * Update pitch sharpening "sharp" with quantized gain_pit
    */
   *sharp = gain_pit;
   if ( *sharp > 0.794556F ) {
      *sharp = 0.794556F;
   }

   /* Find the total excitation */
   for ( i = 0; i < L_SUBFR; i += 4 ) {
      exc[i + i_subfr] = (Float32)floor((gain_pit * exc[i + i_subfr] + gain_code * code[i]) + 0.5F);
      exc[i + i_subfr + 1] = (Float32)floor((gain_pit * exc[i + i_subfr + 1] + gain_code * code[i
            + 1]) + 0.5F);
      exc[i + i_subfr + 2] = (Float32)floor((gain_pit * exc[i + i_subfr + 2] + gain_code * code[
            i + 2]) + 0.5F);
      exc[i + i_subfr + 3] = (Float32)floor((gain_pit * exc[i + i_subfr + 3] + gain_code * code[
            i + 3]) + 0.5F);
   }


   /* The local synthesis speech */
   Syn_filt( a_q, &exc[i_subfr], &synth[i_subfr], mem_syn, 1 );

   for ( i = L_SUBFR - M, j = 0; i < L_SUBFR; i++, j++ ) {
      /* e(n) = S(n) - S_s(n) */
      mem_err[j] = speech[i_subfr + i] - synth[i_subfr + i];

      /* ew(n) = x(n) - Gp_q * y(n) - Gc_q(n) * z(n) */
      mem_w0[j] = xn[i] - y1[i] * gain_pit - y2[i] * gain_code;
   }
}

/*
 * dtx_buffer
 *
 *
 * Parameters:
 *    hist_ptr       B: Circular buffer pointer
 *    lsp_hist       O: LSP history
 *    lsp_new        I: LSP vector
 *    speech         I: input speech
 *    log_en_hist    O: frame energy
 *
 * Function:
 *    Handles the DTX buffer
 *
 *    The frame energy is computed for each frame marked with VAD=0
 *    according to the equation:
 *
 *                                       N-1
 *    en_log(i) = 0.5 * log2 ( (1 / N) * SUM[ s(n) * s(n) ] )
 *                                       N=0
 *
 *    where s(n) is the HP-filtered input speech signal of
 *    the current frame i.
 *
 * Returns:
 *    void
 */
static void dtx_buffer( Word16 *hist_ptr, Float32 *lsp_hist, Float32 lsp_new[],
      Float32 speech[], Float32 *log_en_hist )
{
   Float64 frame_en;


   /* update pointer to circular buffer */
   *hist_ptr += 1;

   if ( *hist_ptr == DTX_HIST_SIZE ) {
      *hist_ptr = 0;
   }

   /* copy lsp vector into buffer */
   memcpy( &lsp_hist[ * hist_ptr * M], lsp_new, sizeof( Float32 )*M );

   /* compute log energy based on frame energy */
   frame_en = Dotproduct40( speech, speech );
   frame_en += Dotproduct40( &speech[40], &speech[40] );
   frame_en += Dotproduct40( &speech[80], &speech[80] );
   frame_en += Dotproduct40( &speech[120], &speech[120] );

   if ( frame_en > 1 ) {
      log_en_hist[ * hist_ptr] = ( Float32 )( log10( frame_en * 0.00625F )*
            1.660964F );
   }
   else {
      log_en_hist[ * hist_ptr] = -3.660965F;
   }
}

/*
 * complex_estimate_adapt
 *
 *
 * Parameters:
 *    st->best_corr_hp  I: complex background detector
 *    st->corr_hp_fast  B: long term complex signal estimate
 *    low_power         I: very low level flag of the input frame
 *
 * Function:
 *    Update/adapt of complex signal estimate
 *
 * Returns:
 *    void
 */
#ifndef VAD2
#endif

/*
 * complex_vad
 *
 *
 * Parameters:
 *    st->complex_high        B: 1 if (corr_hp_fast > CVAD_THRESH_ADAPT_HIGH)
 *    st->complex_low         B: 1 if (corr_hp_fast > CVAD_THRESH_ADAPT_LOW)
 *    low_power               I: flag power of the input frame
 *    st->best_corr_hp        I: complex background detector
 *    st->corr_hp_fast        B: long term complex signal estimate
 *    st->complex_hang_timer  B: complex hang timer
 *
 *
 * Function:
 *    Complex background decision
 *
 * Returns:
 *    void
 */
#ifndef VAD2

#endif

/*
 * complex_vad
 *
 *
 * Parameters:
 *    st->complex_warning  I: flags for complex detection
 *    st->stat_count       B: stationary counter
 *    st->pitch            I: flags for pitch detection
 *    st->tone             I: flags indicating presence of a tone
 *    st->vadreg           I: intermediate VAD flags
 *    level                I: sub-band levels of the input frame
 *    st->ave_level        B: Average amplitude estimate
 *
 * Function:
 *    Control update of the background noise estimate
 *
 * Returns:
 *    void
 */
#ifndef VAD2
#endif

/*
 * noise_estimate_update
 *
 *
 * Parameters:
 *    st                      B: State struct
 *    level                   I: sub-band levels of the input frame
 *    st->vadreg              I: intermediate VAD flags
 *    st->pitch               I: flags for pitch detection
 *    st->complex_hang_count  I: signal is too complex for VAD
 *    st->stat_count          B: stationary counter
 *    st->old_level           B: signal levels of the previous frame
 *    st->bckr_est            B: noise estimate
 *
 * Function:
 *    Update of background noise estimate
 *
 * Returns:
 *    void
 */
#ifndef VAD2
#endif

/*
 * hangover_addition
 *
 *
 * Parameters:
 *    noise_level             I: average level of the noise estimates
 *    low_power               I: flag power of the input frame
 *    st->burst_count         O: counter for the length of speech bursts
 *    st->hang_count          O: hangover counter
 *    st->complex_hang_count  B: signal is too complex for VAD
 *    st->complex_hang_timer  B: complex hang timer
 *    st->vadreg              I: intermediate VAD flags
 *    st->corr_hp_fast        I: long term complex signal estimate
 *
 * Function:
 *    Add hangover for complex signal or after speech bursts
 *
 * Returns:
 *    VAD_flag indicating final VAD decision
 */
#ifndef VAD2
#endif

/*
 * vad_decision
 *
 *
 * Parameters:
 *    st                      B: State struct
 *    level                   I: sub-band levels of the input frame
 *    pow_sum                 I: power of the input frame
 *    st->bckr_est            I: background noise components
 *    st->vadreg              I: intermediate VAD flags
 *    st->complex_warning     O: flags for complex detection
 *    st->speech_vad_decision O: speech VAD flag
 *
 * Function:
 *    Calculates VAD_flag
 *
 * Returns:
 *    VAD_flag indicating final VAD decision
 */
#ifndef VAD2
#endif

/*
 * level_calculation
 *
 *
 * Parameters:
 *    data              I: signal buffer
 *    sub_level         B: level calculate at the end of the previous frame/
 *                         level of signal calculated from the last
 *                         (count2 - count1) samples
 *    count1            I: number of samples to be counted
 *    count2            I: number of samples to be counted
 *    ind_m             I: step size for the index of the data buffer
 *    ind_a             I: starting index of the data buffer
 *    scale             I: scaling for the level calculation
 *
 * Function:
 *    Calculate signal level in a sub-band.
 *
 *    Level is calculated by summing absolute values of the input data.
 *
 * Returns:
 *    signal level
 */
#ifndef VAD2
#endif

/*
 * filter3
 *
 *
 * Parameters:
 *    in0               B: input values; output low-pass part
 *    in1               B: input values; output high-pass part
 *    data              B: updated filter memory
 *
 * Function:
 *    Third-order half-band lowpass/highpass filter pair.
 *
 * Returns:
 *   void
 */
#ifndef VAD2
#endif

/*
 * filter5
 *
 *
 * Parameters:
 *    in0               B: input values; output low-pass part
 *    in1               B: input values; output high-pass part
 *    data              B: updated filter memory
 *
 * Function:
 *    Fifth-order half-band lowpass/highpass filter pair.
 *
 * Returns:
 *   void
 */
#ifndef VAD2
#endif

/*
 * first_filter_stage
 *
 *
 * Parameters:
 *    in                I: input signal
 *    out               O: output values,
 *                         every other output is low-pass part and
 *                         every other output is high-pass part
 *    data              B: updated filter memory
 *
 * Function:
 *    Calculate 5th order half-band lowpass/highpass filter pair
 *
 * Returns:
 *   void
 */
#ifndef VAD2
#endif

/*
 * filter_bank
 *
 *
 * Parameters:
 *    in                I: input frame
 *    st->a_data5       B: filter memory
 *    st->a_data3       B: filter memory
 *    st->sub_level     B: level memory
 *    level             O: signal levels at each band
 *
 * Function:
 *    Divides input signal into 9-bands and calcultes level of the signal in each band
 *
 * Returns:
 *    void
 */
#ifndef VAD2
#endif

/*
 * vad
 *
 *
 * Parameters:
 *    in_buf            I: samples of the input frame
 *    st                B: State struct
 *    st->pitch         B: flags for pitch detection
 *    st->complex_low   B: complex flag
 *
 * Function:
 *    Voice Activity Detection (VAD)
 *
 * Returns:
 *    VAD Decision, 1 = speech, 0 = noise
 */
#ifndef VAD2
#endif

/*
 * vad_pitch_detection
 *
 *
 * Parameters:
 *    st->oldlag        B: old LTP lag
 *    T_op              I: speech encoder open loop lags
 *    st->pitch         B: flags for pitch detection
 *    st                B: State struct
 *    st->pitch         B: flags for pitch detection
 *    st->oldlag_count  B: lag count
 *
 * Function:
 *    Test if signal contains pitch or other periodic component.
 *
 * Returns:
 *    Boolean voiced / unvoiced decision in state variable
 */
#ifndef VAD2
static void vad_pitch_detection( vadState *st, Word32 T_op[] )
{
   Word32 lagcount, i;


   lagcount = 0;

   for ( i = 0; i < 2; i++ ) {
      if ( abs( st->oldlag - T_op[i] ) < LTHRESH ) {
         lagcount += 1;
      }

      /* Save the current LTP lag */
      st->oldlag = T_op[i];
   }

   /*
    * Make pitch decision.
    * Save flag of the pitch detection to the variable pitch.
    */
   st->pitch = st->pitch >> 1;

   if ( ( st->oldlag_count + lagcount ) >= NTHRESH ) {
      st->pitch = st->pitch | 0x4000;
   }

   /* Update oldlagcount */
   st->oldlag_count = lagcount;
}
#endif


#ifdef VAD2

/***************************************************************************
 *
 *   FUNCTION NAME: vad2()
 *
 *   PURPOSE:
 *     This function provides the Voice Activity Detection function option 2
 *     for the Adaptive Multi-rate (AMR) codec.
 *
 *   INPUTS:
 *
 *     vadState
 *                     pointer to vadState state structure
 *     farray_ptr
 *                     pointer to Float32[80] input array
 *
 *   OUTPUTS:
 *
 *     state variables in vadState are updated
 *
 *   RETURN VALUE:
 *
 *     Word16
 *                     VAD(m) - two successive calls to vad2() yield
 *                     the VAD decision for the 20 ms frame:
 *                     VAD_flag = VAD(m-1) || VAD(m)
 *
 *
 *************************************************************************/

int		vad2 (vadState *st, Float32 *farray_ptr)
{

  /* Static variables */

  /* The channel table is defined below.  In this table, the
     lower and higher frequency coefficients for each of the 16
     channels are specified.  The table excludes the coefficients
     with numbers 0 (DC), 1, and 64 (Foldover frequency).  For
     these coefficients, the gain is always set at 1.0 (0 dB). */

  static int	ch_tbl [NUM_CHAN][2] = {

    { 2,  3},
    { 4,  5},
    { 6,  7},
    { 8,  9},
    {10, 11},
    {12, 13},
    {14, 16},
    {17, 19},
    {20, 22},
    {23, 26},
    {27, 30},
    {31, 35},
    {36, 41},
    {42, 48},
    {49, 55},
    {56, 63}

  };

  /* The voice metric table is defined below.  It is a non-
     linear table with a deadband near zero.  It maps the SNR
     index (quantized SNR value) to a number that is a measure
     of voice quality. */

  static int	vm_tbl [90] = {
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 7, 7, 7,
    8, 8, 9, 9, 10, 10, 11, 12, 12, 13, 13, 14, 15,
    15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 23, 24,
    24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34,
    35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 45,
    46, 47, 48, 49, 50, 50, 50, 50, 50, 50, 50, 50,
    50, 50
  };

  /* hangover as a function of peak SNR (3 dB steps) */
  static Word16 hangover_table[20] =
  {
    30, 30, 30, 30, 30, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 8, 8, 8
  };

  /* burst sensitivity as a function of peak SNR (3 dB steps) */
  static Word16 burstcount_table[20] =
  {
    8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4
  };

  /* voice metric sensitivity as a function of peak SNR (3 dB steps) */
  static Word16 vm_threshold_table[20] =
  {
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 40, 51, 71, 100, 139, 191, 257, 337, 432
  };


  /* Automatic variables */

  float		data_buffer [FFT_LEN1], enrg, snr;
  float		tne, tce, ftmp;
  int		ch_snr [NUM_CHAN];
  int		i, j, j1, j2;
  int		vm_sum;
  int		update_flag;

  float		ch_enrg_dev;		/* for forced update... */
  float		ch_enrg_db [NUM_CHAN];
  float		alpha;


  /* For detecting sine waves */
  float		peak, avg, peak2avg;
  int		sine_wave_flag;

  /* For computing frame SNR and long-term SNR */
  float		tce_db, tne_db;
  float		xt;

  /* More VAD stuff */
  int	tsnrq;
  int	ivad;


  /* Functions */

  void real_fft (float *, int);


  /****** Executable code starts here ******/

  /* Increment frame counter */
  st->Lframe_cnt++;

  /* Preemphasize the input data and store in the data buffer with
     appropriate delay */

  for (i = 0; i < DELAY0; i++)
    data_buffer [i] = 0.0;

  data_buffer [DELAY0] = *farray_ptr + PRE_EMP_FAC1 * st->pre_emp_mem;

  for (i = DELAY0+1, j = 1; i < DELAY0+FRM_LEN1; i++, j++)
    data_buffer [i] = *(farray_ptr + j) + PRE_EMP_FAC1 *
      *(farray_ptr + j - 1);

  st->pre_emp_mem = *(farray_ptr + FRM_LEN1 - 1);

  for (i = DELAY0+FRM_LEN1; i < FFT_LEN1; i++)
    data_buffer [i] = 0.0;

  /* Perform FFT on the data buffer */
  real_fft (data_buffer, +1);

  /* Estimate the energy in each channel */
  alpha = (st->Lframe_cnt == 1) ? 1.0 : CEE_SM_FAC1;
  for (i = LO_CHAN; i <= HI_CHAN; i++)
    {
      enrg = 0.0;
      j1 = ch_tbl [i][0], j2 = ch_tbl [i][1];
      for (j = j1; j <= j2; j++)
	enrg += square(data_buffer [2*j]) + square(data_buffer [2*j+1]);
      enrg /= (float) (j2 - j1 + 1);
      st->ch_enrg [i] = (1 - alpha) * st->ch_enrg [i] + alpha * enrg;
      if (st->ch_enrg [i] < MIN_CHAN_ENRG) st->ch_enrg [i] = MIN_CHAN_ENRG;
    }

  /* Compute the total channel energy estimate (tce) */
  tce = 0.0;
  for (i = LO_CHAN; i <= HI_CHAN; i++)
    tce += st->ch_enrg [i];

  /* Calculate spectral peak-to-average ratio */
  peak = avg = 0.;
  for (i = LO_CHAN; i <= HI_CHAN; i++) {
    /* Sine waves not valid for low frequencies: */
    if (i >= SINE_START_CHAN && st->ch_enrg [i] > peak)
      peak = st->ch_enrg [i];
    avg += st->ch_enrg [i];
  }
  avg /= HI_CHAN - LO_CHAN + 1;
  peak2avg = (avg < 1./NORM_ENRG) ? 0. : 10.*log10 (peak/avg);

  /* Detect sine waves */
  if (peak2avg > 10.)
    sine_wave_flag = TRUE;
  else
    sine_wave_flag = FALSE;

  /* Initialize channel noise estimate to channel energy of first few frames
     (if sufficiently low Peak-to-Average ratio) */
  if (st->Lframe_cnt <= INIT_FRAMES) {
    if (sine_wave_flag == TRUE) {
      for (i = LO_CHAN; i <= HI_CHAN; i++)
        st->ch_noise [i] = INE;
    }
    else {
      for (i = LO_CHAN; i <= HI_CHAN; i++)
        st->ch_noise [i] = max(st->ch_enrg [i], INE);
    }
  }

  /* Compute the channel SNR indices */
  for (i = LO_CHAN; i <= HI_CHAN; i++) {
    snr = 10.0 * log10 ((double)st->ch_enrg [i] / st->ch_noise [i]);
    if (snr < 0.0) snr = 0.0;
    ch_snr [i] = (snr + 0.1875) / 0.375;
  }

  /* Compute the sum of voice metrics */
  vm_sum = 0;
  for (i = LO_CHAN; i <= HI_CHAN; i++) {
    j = min(ch_snr[i],89);
    vm_sum += vm_tbl [j];
  }

  /* Initialize voice energy to nominal value */
  if (st->Lframe_cnt <= INIT_FRAMES  || st->fupdate_flag == TRUE ) {
#if NORM_ENERG==4
    tce_db = 49.918;
#elif NORM_ENERG==1
    tce_db = 55.938;
#else
    tce_db = (96. - 22. - 10*log10 (FFT_LEN1/2) - 10.*log10 (NORM_ENRG));
#endif

    st->negSNRvar = 0.0;
    st->negSNRbias = 0.0;

    /* Compute the total noise energy estimate (tne) */
    tne = 0.0;
    for (i = LO_CHAN; i <= HI_CHAN; i++)
      tne += st->ch_noise [i];

    /* Get total noise in dB */
    tne_db = 10 * log10 (tne);

    /* Initialise instantaneous and long-term peak signal-to-noise ratios */
    xt = tce_db - tne_db;
    st->tsnr = xt;

  }
  else {

    /* Calculate instantaneous signal-to-noise ratio */
    xt = 0;
    for (i=LO_CHAN; i<=HI_CHAN; i++)
      xt += st->ch_enrg[i]/st->ch_noise[i];
    xt = 10*log10(xt/NUM_CHAN);

    /* Estimate long-term "peak" SNR */
    if (xt > st->tsnr)
      st->tsnr = 0.9*st->tsnr + 0.1*xt;
    else if (xt > 0.625*st->tsnr)
      st->tsnr = 0.998*st->tsnr + 0.002*xt;
  }

  /* Quantize the long-term SNR in 3 dB steps */
  tsnrq = (int)(st->tsnr/3.);
  tsnrq = min(19, max(0, tsnrq));

  /* Calculate the negative SNR sensitivity bias */
  if (xt < 0) {
    st->negSNRvar = min (0.99*st->negSNRvar + 0.01*xt*xt, 4.0);
    st->negSNRbias = max (12.0*(st->negSNRvar - 0.65), 0.0);
  }

  /* Determine VAD as a function of the voice metric sum and quantized SNR */
  if (vm_sum > vm_threshold_table[tsnrq] + st->negSNRbias) {
    ivad = 1;
    if (++st->burstcount > burstcount_table[tsnrq]) {
      st->hangover = hangover_table[tsnrq];
    }
  } else {
    st->burstcount = 0;
    if (--st->hangover <= 0) {
      ivad = 0;
      st->hangover = 0;
    } else {
      ivad = 1;
    }
  }

  /* Calculate log spectral deviation */
  for (i = LO_CHAN; i <= HI_CHAN; i++)
    ch_enrg_db [i] = 10.*log10( st->ch_enrg [i] );

  ch_enrg_dev = 0.;
  if (st->Lframe_cnt == 1)
    for (i = LO_CHAN; i <= HI_CHAN; i++)
      st->ch_enrg_long_db [i] = ch_enrg_db [i];
  else
    for (i = LO_CHAN; i <= HI_CHAN; i++)
      ch_enrg_dev += fabs( st->ch_enrg_long_db [i] - ch_enrg_db [i] );


  /*
   * Calculate long term integration constant as a function of instantaneous SNR
   * (i.e., high SNR (tsnr dB) -> slower integration (alpha = HIGH_ALPHA),
   *         low SNR (0 dB) -> faster integration (alpha = LOW_ALPHA)
   */

  /* alpha = HIGH_ALPHA - ALPHA_RANGE * (tsnr - xt) / tsnr, low <= alpha <= high */
  ftmp = st->tsnr - xt;
  if (ftmp <= 0.0 || st->tsnr <= 0.0)
    alpha = HIGH_ALPHA1;
  else if (ftmp > st->tsnr)
    alpha = LOW_ALPHA1;
  else
    alpha = HIGH_ALPHA1 - (ALPHA_RANGE1 * ftmp / st->tsnr);

  /* Calc long term log spectral energy */
  for (i = LO_CHAN; i <= HI_CHAN; i++) {
    st->ch_enrg_long_db[i] = alpha*st->ch_enrg_long_db[i] + (1.-alpha)*ch_enrg_db[i];
  }

  /* Set or reset the update flag */
  update_flag = FALSE;
  st->fupdate_flag = FALSE;
  if ((vm_sum <= UPDATE_THLD) ||
      (st->Lframe_cnt <= INIT_FRAMES && sine_wave_flag == FALSE)) {
    update_flag = TRUE;
    st->update_cnt = 0;
  }
  else if (tce > NOISE_FLOOR && ch_enrg_dev < DEV_THLD1 &&
           sine_wave_flag == FALSE && st->LTP_flag == FALSE) {
    st->update_cnt++;
    if (st->update_cnt >= UPDATE_CNT_THLD1) {
      update_flag = TRUE;
      st->fupdate_flag = TRUE;
    }
  }

  if ( st->update_cnt == st->last_update_cnt )
    st->hyster_cnt++;
  else
    st->hyster_cnt = 0;
  st->last_update_cnt = st->update_cnt;

  if ( st->hyster_cnt > HYSTER_CNT_THLD1 )
    st->update_cnt = 0;

  /* Update the channel noise estimates */
  if (update_flag == TRUE) {
    for (i = LO_CHAN; i <= HI_CHAN; i++) {
      st->ch_noise [i] = (1.0 - CNE_SM_FAC1) * st->ch_noise [i] +
	CNE_SM_FAC1 * st->ch_enrg [i];
      if (st->ch_noise [i] < MIN_CHAN_ENRG) st->ch_noise [i] = MIN_CHAN_ENRG;
    }
  }

  return (ivad);

}		/* end vad2 () */


/**************************************************************************
 *
 *   FUNCTION NAME: real_fft()
 *
 *   PURPOSE: FFT/IFFT function for real sequences
 *
 **************************************************************************
 *
 * This is an implementation of decimation-in-time FFT algorithm for
 * real sequences.  The techniques used here can be found in several
 * books, e.g., i) Proakis and Manolakis, "Digital Signal Processing",
 * 2nd Edition, Chapter 9, and ii) W.H. Press et. al., "Numerical
 * Recipes in C", 2nd Ediiton, Chapter 12.
 *
 * Input -  There is two inputs to this function:
 *
 *	1) A float pointer to the input data array,
 *	2) A control parameter (isign) specifying forward (+1) or
 *         inverse (-1) FFT.
 *
 * Output - There is no return value.
 *	The input data are replaced with transformed data.  If the
 *	input is a real time domain sequence, it is replaced with
 *	the complex FFT for positive frequencies.  The FFT value
 *	for DC and the foldover frequency are combined to form the
 *	first complex number in the array.  The remaining complex
 *	numbers correspond to increasing frequencies.  If the input
 *	is a complex frequency domain sequence arranged	as above,
 *	it is replaced with the corresponding time domain sequence.
 *
 * Notes:
 *
 *	1) This function is designed to be a part of a VAD
 *	   algorithm that requires 128-point FFT of real
 *	   sequences.  This is achieved here through a 64-point
 *	   complex FFT.  Consequently, the FFT size information is
 *	   not transmitted explicitly.  However, some flexibility
 *	   is provided in the function to change the size of the
 *	   FFT by specifying the size information through "define"
 *	   statements.
 *
 *	2) The values of the complex sinusoids used in the FFT
 *	   algorithm are stored in a ROM table.
 *
 *	3) In the c_fft function, the FFT values are divided by
 *	   2 after each stage of computation thus dividing the
 *	   final FFT values by 64.  This is somewhat different
 *         from the usual definition of FFT where the factor 1/N,
 *         i.e., 1/64, used for the IFFT and not the FFT.  No factor
 *         is used in the r_fft function.
 *
 *************************************************************************/

static double	phs_tbl [SIZE];		/* holds the complex sinusoids */

void		real_fft (float *farray_ptr, int isign)
{

  float		ftmp1_real, ftmp1_imag, ftmp2_real, ftmp2_imag;
  int		i, j;
  static int	first = TRUE;

  void		cmplx_fft (float *, int);
  void		fill_tbl ();

  /* If this is the first call to the function, fill up the
     phase table  */
  if (first == TRUE) {
    fill_tbl ();
    first = FALSE;
  }

  /* The FFT part */
  if (isign == 1) {

    /* Perform the complex FFT */
    cmplx_fft (farray_ptr, isign);

    /* First, handle the DC and foldover frequencies */
    ftmp1_real = *farray_ptr;
    ftmp2_real = *(farray_ptr + 1);
    *farray_ptr = ftmp1_real + ftmp2_real;
    *(farray_ptr + 1) = ftmp1_real - ftmp2_real;

    /* Now, handle the remaining positive frequencies */
    for (i = 2, j = SIZE - i; i <= SIZE_BY_TWO; i = i + 2, j = SIZE - i) {

      ftmp1_real = *(farray_ptr + i) + *(farray_ptr + j);
      ftmp1_imag = *(farray_ptr + i + 1) - *(farray_ptr + j + 1);
      ftmp2_real = *(farray_ptr + i + 1) + *(farray_ptr + j + 1);
      ftmp2_imag = *(farray_ptr + j) - *(farray_ptr + i);

      *(farray_ptr + i) = (ftmp1_real + phs_tbl [i] * ftmp2_real -
			   phs_tbl [i + 1] * ftmp2_imag) / 2.0;
      *(farray_ptr + i + 1) = (ftmp1_imag + phs_tbl [i] * ftmp2_imag +
			       phs_tbl [i + 1] * ftmp2_real) / 2.0;
      *(farray_ptr + j) = (ftmp1_real + phs_tbl [j] * ftmp2_real +
			   phs_tbl [j + 1] * ftmp2_imag) / 2.0;
      *(farray_ptr + j + 1) = (-ftmp1_imag - phs_tbl [j] * ftmp2_imag +
			       phs_tbl [j + 1] * ftmp2_real) / 2.0;
    }
  }

  /* The IFFT part */
  else {

    /* First, handle the DC and foldover frequencies */

    ftmp1_real = *farray_ptr;
    ftmp2_real = *(farray_ptr + 1);
    *farray_ptr = (ftmp1_real + ftmp2_real) / 2.0;
    *(farray_ptr + 1) = (ftmp1_real - ftmp2_real) / 2.0;

    /* Now, handle the remaining positive frequencies */

    for (i = 2, j = SIZE - i; i <= SIZE_BY_TWO; i = i + 2, j = SIZE - i) {

      ftmp1_real = *(farray_ptr + i) + *(farray_ptr + j);
      ftmp1_imag = *(farray_ptr + i + 1) - *(farray_ptr + j + 1);
      ftmp2_real = -(*(farray_ptr + i + 1) + *(farray_ptr + j + 1));
      ftmp2_imag = -(*(farray_ptr + j) - *(farray_ptr + i));

      *(farray_ptr + i) = (ftmp1_real + phs_tbl [i] * ftmp2_real +
			   phs_tbl [i + 1] * ftmp2_imag) / 2.0;
      *(farray_ptr + i + 1) = (ftmp1_imag + phs_tbl [i] * ftmp2_imag -
			       phs_tbl [i + 1] * ftmp2_real) / 2.0;
      *(farray_ptr + j) = (ftmp1_real + phs_tbl [j] * ftmp2_real -
			   phs_tbl [j + 1] * ftmp2_imag) / 2.0;
      *(farray_ptr + j + 1) = (-ftmp1_imag - phs_tbl [j] * ftmp2_imag -
			       phs_tbl [j + 1] * ftmp2_real) / 2.0;
    }

    /* Perform the complex IFFT */
    cmplx_fft (farray_ptr, isign);
  }

  return;
}		/* end real_fft () */



/*
 * FFT/IFFT function for complex sequences
 *
 * The decimation-in-time complex FFT/IFFT is implemented below.
 * The input complex numbers are presented as real part followed by
 * imaginary part for each sample.  The counters are therefore
 * incremented by two to access the complex valued samples.
 */
void		cmplx_fft (float *farray_ptr, int isign)
{
  int		i, j, k, ii, jj, kk, ji, kj;
  float		ftmp, ftmp_real, ftmp_imag;

  /* Rearrange the input array in bit reversed order */
  for (i = 0, j = 0; i < SIZE-2; i = i + 2) {
    if (j > i) {
      ftmp = *(farray_ptr+i);
      *(farray_ptr+i) = *(farray_ptr+j);
      *(farray_ptr+j) = ftmp;

      ftmp = *(farray_ptr+i+1);
      *(farray_ptr+i+1) = *(farray_ptr+j+1);
      *(farray_ptr+j+1) = ftmp;
    }
    k = SIZE_BY_TWO;
    while (j >= k) {
      j -= k;
      k >>= 1;
    }
    j += k;
  }

  /* The FFT part */
  if (isign == 1) {
    for (i = 0; i < NUM_STAGE; i++) {		/* i is stage counter */
      jj = (2 << i);				/* FFT size */
      kk = (jj << 1);				/* 2 * FFT size */
      ii = SIZE / jj;				/* 2 * number of FFT's */
      for (j = 0; j < jj; j = j + 2) {		/* j is sample counter */
        ji = j * ii;				/* ji is phase table index */
        for (k = j; k < SIZE; k = k + kk) {	/* k is butterfly top */
          kj = k + jj;				/* kj is butterfly bottom */

	  /* Butterfly computations */
          ftmp_real = *(farray_ptr + kj) * phs_tbl [ji] -
	    *(farray_ptr + kj + 1) * phs_tbl [ji + 1];

          ftmp_imag = *(farray_ptr + kj + 1) * phs_tbl [ji] +
	    *(farray_ptr + kj) * phs_tbl [ji + 1];

          *(farray_ptr + kj) = (*(farray_ptr + k) - ftmp_real) / 2.0;
          *(farray_ptr + kj + 1) = (*(farray_ptr + k + 1) - ftmp_imag) / 2.0;

          *(farray_ptr + k) = (*(farray_ptr + k) + ftmp_real) / 2.0;
          *(farray_ptr + k + 1) = (*(farray_ptr + k + 1) + ftmp_imag) / 2.0;
        }
      }
    }
  }

  /* The IFFT part */
  else {
    for (i = 0; i < NUM_STAGE; i++) {		/* i is stage counter */
      jj = (2 << i);				/* FFT size */
      kk = (jj << 1);				/* 2 * FFT size */
      ii = SIZE / jj;				/* 2 * number of FFT's */
      for (j = 0; j < jj; j = j + 2) {		/* j is sample counter */
        ji = j * ii;				/* ji is phase table index */
        for (k = j; k < SIZE; k = k + kk) {	/* k is butterfly top */
          kj = k + jj;				/* kj is butterfly bottom */

	  /* Butterfly computations */
          ftmp_real = *(farray_ptr + kj) * phs_tbl [ji] +
	    *(farray_ptr + kj + 1) * phs_tbl [ji + 1];

          ftmp_imag = *(farray_ptr + kj + 1) * phs_tbl [ji] -
	    *(farray_ptr + kj) * phs_tbl [ji + 1];

          *(farray_ptr + kj) = *(farray_ptr + k) - ftmp_real;
          *(farray_ptr + kj + 1) = *(farray_ptr + k + 1) - ftmp_imag;

          *(farray_ptr + k) = *(farray_ptr + k) + ftmp_real;
          *(farray_ptr + k + 1) = *(farray_ptr + k + 1) + ftmp_imag;
        }
      }
    }
  }
  return;
}		/* end of cmplx_fft () */


/* Function to fill the phase table values
 */

void		fill_tbl ()
{
  int		i;
  double	delta_f, theta;

  delta_f = - PI / (double) SIZE_BY_TWO;
  for (i = 0; i < SIZE_BY_TWO; i++) {
    theta = delta_f * (double) i;
    phs_tbl[2*i] = cos(theta);
    phs_tbl[2*i+1] = sin(theta);
  }
  return;
}		/* end fill_tbl () */


/***************************************************************************
 *
 *   FUNCTION NAME: LTP_flag_update
 *
 *   PURPOSE:
 *     Set LTP_flag if the LTP gain > LTP_THRESHOLD, where the value of
 *     LTP_THRESHOLD depends on the LTP analysis window length.
 *
 *   INPUTS:
 *
 *     mode
 *                     AMR mode
 *     vadState->R0
 *                     LTP energy
 *     vadState->Rmax
 *                     LTP maximum autocorrelation
 *   OUTPUTS:
 *
 *     vadState->LTP_flag
 *                     Set if LTP gain > LTP_THRESHOLD
 *
 *   RETURN VALUE:
 *
 *     none
 *
 *************************************************************************/

void LTP_flag_update (vadState * st, Word16 mode)
{
  Float32 thresh;


  thresh = 0.65;

  if (st->Rmax  > thresh*st->R0)
    st->LTP_flag = TRUE;
  else
    st->LTP_flag = FALSE;

  return;
}

/***************************************************************************/
#endif

/*
 * cod_amr
 *
 *
 * Parameters:
 *    st          B: state structure
 *    mode        I: encoder mode
 *    new_speech  I: input speech frame, size L_FRAME
 *    st          B: State struct
 *    ana         O: Analysis parameters
 *    used_mode   B: In: -1 forces VAD on, Out:used encoder mode
 *    synth       O: local synthesis, size L_FRAME
 *
 * Function:
 *    GSM adaptive multi rate speech encoder
 *
 * Returns:
 *    void
 */
static void cod_amr( cod_amrState *st, enum Mode mode, Float32 new_speech[],
      Word16 ana[], enum Mode *used_mode, Float32 synth[] )
{
   /* LPC coefficients */
   Float32 A_t[( MP1 ) * 4];   /* A(z) unquantized for the 4 subframes */
   Float32 Aq_t[( MP1 ) * 4];   /* A(z)   quantized for the 4 subframes */
   Float32 *A, *Aq;   /* Pointer on Aq_t */
   Float32 lsp_new[M];


   /* Other vectors */
   Float32 xn[L_SUBFR];   /* Target vector for pitch search */
   Float32 xn2[L_SUBFR];   /* Target vector for codebook search */
   Float32 code[L_SUBFR];   /* Fixed codebook excitation */
   Float32 y1[L_SUBFR];   /* Filtered adaptive excitation */
   Float32 y2[L_SUBFR];   /* Filtered fixed codebook excitation */
   Float32 gCoeff[3];   /* Correlations between xn, y1, & y2: */
   Float32 res[L_SUBFR];   /* Short term (LPC) prediction residual */
   Float32 res2[L_SUBFR];   /* Long term (LTP) prediction residual */


   /* Vector and scalars needed for the MR475 */
   Float32 gain_pit_sf0;   /* Quantized pitch gain for sf0 */
   Float32 gain_code_sf0;   /* Quantized codebook gain for sf0 */


   /* Scalars & Flags */
   Float32 gain_pit, gain_code;
   Float32 gp_limit;   /* pitch gain limit value */
   Word32 T0, T0_frac;
   Word32 T_op[2];
   Word32 evenSubfr;
   Word32 i;
   Word16 i_subfr, subfrNr;
   Word16 lsp_flag = 0;   /* indicates resonance in LPC filter */


   memcpy( st->new_speech, new_speech, L_FRAME <<2 );

   if ( st->dtx ) {
#ifdef VAD2
     /* Find VAD decision (option 2) */
     vad_flag = vad2 (st->vadSt, st->new_speech);
     vad_flag = vad2 (st->vadSt, st->new_speech+80) || vad_flag;
#else
      /* Find VAD decision (option 1) */
#endif
      /* force VAD on   */

      *used_mode = mode;

      /* NB! used_mode may change here */
   }
   else {
      *used_mode = mode;
   }

   /*
    * Perform LPC analysis:
    * Autocorrelation + Lag windowing.
    * Levinson-durbin algorithm to find a[].
    * Convert a[] to lsp[].
    * Quantize and code the LSPs.
    * find the interpolated LSPs and convert to a[] for all
    * subframes (both quantized and unquantized).
    */
   /* LP analysis */
   lpc( st->lpcSt->LevinsonSt->old_A, st->p_window, st->p_window_12k2, A_t, mode
         );

   /*
    * The LP filter coefficients, are converted to
    * the line spectral pair (LSP) representation for
    * quantization and interpolation purposes.
    */
   lsp( mode, *used_mode, st->lspSt->lsp_old, st->lspSt->lsp_old_q, st->lspSt->
         qSt->past_rq, A_t, Aq_t, lsp_new, &ana );

   /* Buffer lsp's and energy */
   dtx_buffer( &st->dtxEncSt->hist_ptr, st->dtxEncSt->lsp_hist, lsp_new, st->
         new_speech, st->dtxEncSt->log_en_hist );

   {
      /* check resonance in the filter */
      lsp_flag = check_lsp( &st->tonStabSt->count, st->lspSt->lsp_old );
   }

#ifdef VAD2
   if (st->dtx) {
      st->vadSt->Rmax = 0.0;
      st->vadSt->R0 = 0.0;
   }
#endif

   for ( subfrNr = 0, i_subfr = 0; subfrNr < 2; subfrNr++, i_subfr +=
         L_FRAME_BY2 ) {
      /*
       * Pre-processing on 80 samples
       * Find the weighted input speech for the whole speech frame
       */
      pre_big( mode, gamma1, gamma1_12k2, gamma2, A_t, i_subfr, st->speech, st->
            mem_w, st->wsp );

      /* Find open loop pitch lag for two subframes */
      if ( ( mode != MR475 ) && ( mode != MR515 ) ) {
         ol_ltp( mode, st->vadSt, &st->wsp[i_subfr], &T_op[subfrNr], st->
               ol_gain_flg, &st->pitchOLWghtSt->old_T0_med, &st->pitchOLWghtSt->
               wght_flg, &st->pitchOLWghtSt->ada_w, st->old_lags, st->dtx,
               subfrNr );
      }
   }

#ifdef VAD2
   if (st->dtx) {
      LTP_flag_update(st->vadSt, mode);
   }
#endif

#ifndef VAD2
   if ( st->dtx ) {
      vad_pitch_detection( st->vadSt, T_op );
   }
#endif


   /*
    * Loop for every subframe in the analysis frame
    *
    * To find the pitch and innovation parameters. The subframe size is
    * L_SUBFR and the loop is repeated L_FRAME/L_SUBFR times.
    *     - find the weighted LPC coefficients
    *     - find the LPC residual signal res[]
    *     - compute the target signal for pitch search
    *     - compute impulse response of weighted synthesis filter (h1[])
    *     - find the closed-loop pitch parameters
    *     - encode the pitch dealy
    *     - update the impulse response h1[] by including fixed-gain pitch
    *     - find target vector for codebook search
    *     - codebook search
    *     - encode codebook address
    *     - VQ of pitch and codebook gains
    *     - find synthesis speech
    *     - update states of weighting filter
    */
   /* pointer to interpolated LPC parameters */
   A = A_t;

   /* pointer to interpolated quantized LPC parameters */
   Aq = Aq_t;
   evenSubfr = 0;
   subfrNr = -1;

   for ( i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR ) {
      subfrNr += 1;
      evenSubfr = 1 - evenSubfr;

      /* Preprocessing of subframe */
      if ( *used_mode != MR475 ) {
         subframePreProc( *used_mode, gamma1, gamma1_12k2, gamma2, A, Aq, &st->
               speech[i_subfr], st->mem_err, st->mem_w0, st->zero, st->ai_zero,
               &st->exc[i_subfr], st->h1, xn, res, st->error );
      }

      /* copy the LP residual (res2 is modified in the CL LTP search) */
      memcpy( res2, res, L_SUBFR <<2 );

      /* Closed-loop LTP search */
      cl_ltp( &st->clLtpSt->pitchSt->T0_prev_subframe, st->tonStabSt->gp, *
            used_mode, i_subfr, T_op, st->h1, &st->exc[i_subfr], res2, xn,
            lsp_flag, xn2, y1, &T0, &T0_frac, &gain_pit, gCoeff, &ana, &gp_limit
            );

      /* update LTP lag history */
      if ( ( subfrNr == 0 ) && ( st->ol_gain_flg[0] > 0 ) ) {
         st->old_lags[1] = T0;
      }

      if ( ( subfrNr == 3 ) && ( st->ol_gain_flg[1] > 0 ) ) {
         st->old_lags[0] = T0;
      }

      /* Innovative codebook search (find index and gain) */
      cbsearch( *used_mode, subfrNr, xn2, st->h1, T0, st->sharp, gain_pit, code,
            y2, res2, &ana );

      /* Quantization of gains. */
      gainQuant( *used_mode, evenSubfr, st->gainQuantSt->gc_predSt->past_qua_en,
            st->gainQuantSt->gc_predUncSt->past_qua_en, st->gainQuantSt->
            sf0_coeff, &st->gainQuantSt->sf0_target_en, &st->gainQuantSt->
            sf0_gcode0_exp, &st->gainQuantSt->
            sf0_gcode0_fra, &st->gainQuantSt->gain_idx_ptr, &gain_pit_sf0, &
            gain_code_sf0, res, &st->exc[i_subfr], code, xn, xn2, y1, y2, gCoeff
            , gp_limit, &gain_pit, &gain_code, &st->gainQuantSt->adaptSt->
            prev_gc, &st->gainQuantSt->adaptSt->onset, st->gainQuantSt->adaptSt
            ->ltpg_mem, &st->gainQuantSt->adaptSt->prev_alpha, &ana );

      /* update gain history */
      for ( i = 0; i < N_FRAME - 1; i++ ) {
         st->tonStabSt->gp[i] = st->tonStabSt->gp[i + 1];
      }
      st->tonStabSt->gp[N_FRAME - 1] = gain_pit;

      /* Subframe Post Processing */
      if ( *used_mode != MR475 ) {
         subframePostProc( st->speech, i_subfr, gain_pit, gain_code, Aq, synth,
               xn, code, y1, y2, st->mem_syn, st->mem_err, st->mem_w0, st->exc,
               &st->sharp );
      }
      
      /* interpolated LPC parameters for next subframe */
      A += MP1;
      Aq += MP1;
   }

   /* Update signal for next frame. */
   for ( i = 0; i < PIT_MAX; i++ ) {
      st->old_wsp[i] = st->old_wsp[L_FRAME + i];
   }

   for ( i = 0; i < PIT_MAX + L_INTERPOL; i++ ) {
      st->old_exc[i] = st->old_exc[L_FRAME + i];
   }

   for ( i = 0; i < L_TOTAL - L_FRAME; i++ ) {
      st->old_speech[i] = st->old_speech[L_FRAME + i];
   }
}


/*
 * Pre_Process_reset
 *
 *
 * Parameters:
 *    state                O: state structure
 *
 * Function:
 *    Initializes state memory to zero
 *
 * Returns:
 *
 */
static Word32 Pre_Process_reset( Pre_ProcessState *state )
{
   if ( state == ( Pre_ProcessState * )NULL ) {
      DebugPrintf("Pre_Process_reset: invalid parameter\n" );
      return-1;
   }
   state->y2 = 0;
   state->y1 = 0;
   state->x0 = 0;
   state->x1 = 0;
   return 0;
}


/*
 * Pre_Process_exit
 *
 *
 * Parameters:
 *    state             I: state structure
 *
 * Function:
 *    The memory used for state memory is freed
 *
 * Returns:
 *    Void
 */
static void Pre_Process_exit( Pre_ProcessState **state )
{
   if ( state == NULL || *state == NULL )
      return;

   /* deallocate memory */
   free( *state );
   *state = NULL;
   return;
}


/*
 * Pre_Process_init
 *
 *
 * Parameters:
 *    state             O: state structure
 *
 * Function:
 *    Allocates state memory and initializes state memory
 *
 * Returns:
 *    succeed = 0
 */
static Word32 Pre_Process_init( Pre_ProcessState **state )
{
   Pre_ProcessState * s;

   if ( state == ( Pre_ProcessState * * )NULL ) {
      DebugPrintf("Pre_Process_init: invalid parameter\n" );
      return-1;
   }
   *state = NULL;

   /* allocate memory */
   if ( ( s = ( Pre_ProcessState * ) malloc( sizeof( Pre_ProcessState ) ) ) ==
         NULL ) {
      DebugPrintf("Pre_Process_init: can not malloc state structure\n" );
      return-1;
   }
   Pre_Process_reset( s );
   *state = s;
   return 0;
}


/*
 * Pre_Process
 *
 *
 * Parameters:
 *    y2, y1, x0, x1    B: filter memory
 *    speech            I: speech vector to be processed
 *    fspeech           O: processed vector
 *    size              I: size of the vector
 *
 * Function:
 *    Pre-processing
 *
 *    Two pre-processing functions are applied prior to
 *    the encoding process: high-pass filtering and signal down-scaling.
 *    Down-scaling consists of dividing the input by a factor of 2
 *    to reduce the possibility of overflows in the fixed-point
 *    implementation. The high-pass filter serves as a precaution
 *    against undesired low frequency components. A filter with
 *    a cut off frequency of 80 Hz is used, and it is given by:
 *
 *            0.927246093 - 1.8544941z^-1 + 0.927246903z^-2
 *    H(z) = -----------------------------------------------
 *                1 - 1.906005859z^-1 + 0.911376953z^-2
 *
 *    Down-scaling and high-pass filtering are combined by dividing
 *    the coefficients at the numerator by 2.
 *
 * Returns:
 *    void
 */
static void Pre_Process( Float32 *y2, Float32 *y1, Float32 *x0, Float32
      *x1, Word16 *speech, Float32 *f_speech )
{
   Word32 i;
   Float32 x2;
   Float32 tmp;


   for ( i = 0; i < 160; i++ ) {
      x2 = *x1;
      *x1 = *x0;
      *x0 = speech[i];
      tmp = ( Float32 )( 0.4636230465* *x0 - 0.92724705 * *x1 + 0.4636234515 *
            x2 + 1.906005859 * *y1 - 0.911376953 * *y2 );
      f_speech[i] = tmp;
      *y2 = *y1;
      *y1 = tmp;
   }

   if ( ( fabs( *y1 )+fabs( *y2 ) ) < 0.0000000001 )
      *y2 = *y1 = 0;
}


/*
 * cod_amr_reset
 *
 *
 * Parameters:
 *    s                 B: state structure
 *    dtx               I: dtx on/off
 *
 * Function:
 *    Resets state memory
 *
 * Returns:
 *    void
 */
static void cod_amr_reset( cod_amrState *s, Word32 dtx )
{
   Word32 i;


   /* reset DTX */
   s->dtx = dtx;

   /* reset Pitch_frState */
   s->clLtpSt->pitchSt->T0_prev_subframe = 0;

   /* reset Q_plsfState */
   memset( s->lspSt->qSt->past_rq, 0, sizeof( Float32 )*M );
   memcpy( s->lspSt->lsp_old, lsp_init_data, sizeof( lsp_init_data ) );
   memcpy( s->lspSt->lsp_old_q, lsp_init_data, sizeof( lsp_init_data ) );

   /* reset gc_predState */
   for ( i = 0; i < NPRED; i++ ) {
      s->gainQuantSt->gc_predSt->past_qua_en[i] = NB_QUA_CODE+VQ_SIZE_HIGHRATES+VQ_SIZE_LOWRATES+MR475_VQ_SIZE*2+DTX_VQ_SIZE;
      s->gainQuantSt->gc_predUncSt->past_qua_en[i] = NB_QUA_CODE+VQ_SIZE_HIGHRATES+VQ_SIZE_LOWRATES+MR475_VQ_SIZE*2+DTX_VQ_SIZE;
   }

   /* reset gain_adaptState */
   s->gainQuantSt->adaptSt->onset = 0;
   s->gainQuantSt->adaptSt->prev_alpha = 0.0F;
   s->gainQuantSt->adaptSt->prev_gc = 0.0F;
   memset( s->gainQuantSt->adaptSt->ltpg_mem, 0, sizeof( Float32 )*LTPG_MEM_SIZE
         );
   s->gainQuantSt->sf0_gcode0_exp = 0;
   s->gainQuantSt->sf0_gcode0_fra = 0;
   s->gainQuantSt->sf0_target_en = 0.0F;
   memset( s->gainQuantSt->sf0_coeff, 0, sizeof( Float32 )*5 );
   s->gainQuantSt->gain_idx_ptr = NULL;

   /* reset pitchOLWghtState */
   s->pitchOLWghtSt->old_T0_med = 40;
   s->pitchOLWghtSt->ada_w = 0.0F;
   s->pitchOLWghtSt->wght_flg = 0;

   /* reset tonStabState */
   s->tonStabSt->count = 0;
   memset( s->tonStabSt->gp, 0, sizeof( Float32 )*N_FRAME );

   /* reset LevinsonState */
   s->lpcSt->LevinsonSt->old_A[0] = 1.0F;
   memset( &s->lpcSt->LevinsonSt->old_A[1], 0, sizeof( Float32 )*M );

#ifdef VAD2
   /* reset vadState */
   s->vadSt->pre_emp_mem = 0.0;
   s->vadSt->update_cnt = 0;
   s->vadSt->hyster_cnt = 0;
   s->vadSt->last_update_cnt = 0;
   for ( i = 0; i < NUM_CHAN; i++ ) {
     s->vadSt->ch_enrg_long_db[i] = 0.0;
     s->vadSt->ch_enrg[i] = 0.0;
     s->vadSt->ch_noise[i] = 0.0;
   }
   s->vadSt->Lframe_cnt = 0L;
   s->vadSt->tsnr = 0.0;
   s->vadSt->hangover = 0;
   s->vadSt->burstcount = 0;
   s->vadSt->fupdate_flag = 0;
   s->vadSt->negSNRvar = 0.0;
   s->vadSt->negSNRbias = 0.0;
   s->vadSt->R0 = 0.0;
   s->vadSt->Rmax = 0.0;
   s->vadSt->LTP_flag = 0;
#else
   /* reset vadState */
   s->vadSt->oldlag_count = 0;
   s->vadSt->oldlag = 0;
   s->vadSt->pitch = 0;
   s->vadSt->tone = 0;
   s->vadSt->complex_high = 0;
   s->vadSt->complex_low = 0;
   s->vadSt->complex_hang_timer = 0;
   s->vadSt->vadreg = 0;
   s->vadSt->burst_count = 0;
   s->vadSt->hang_count = 0;
   s->vadSt->complex_hang_count = 0;

   /* initialize memory used by the filter bank */
   for ( i = 0; i < 3; i++ ) {
      s->vadSt->a_data5[i][0] = 0;
      s->vadSt->a_data5[i][1] = 0;
   }

   for ( i = 0; i < 5; i++ ) {
      s->vadSt->a_data3[i] = 0;
   }

   /* reset dtx_encState */
   /* initialize the rest of the memory */
   for ( i = 0; i < COMPLEN; i++ ) {
      s->vadSt->bckr_est[i] = NOISE_INIT;
      s->vadSt->old_level[i] = NOISE_INIT;
      s->vadSt->ave_level[i] = NOISE_INIT;
      s->vadSt->sub_level[i] = 0;
   }
   s->vadSt->best_corr_hp = CVAD_LOWPOW_RESET;
   s->vadSt->speech_vad_decision = 0;
   s->vadSt->complex_warning = 0;
   s->vadSt->sp_burst_count = 0;
   s->vadSt->corr_hp_fast = CVAD_LOWPOW_RESET;
#endif

   s->dtxEncSt->hist_ptr = 0;
   s->dtxEncSt->log_en_index = 0;
   s->dtxEncSt->init_lsf_vq_index = 0;
   s->dtxEncSt->lsp_index[0] = 0;
   s->dtxEncSt->lsp_index[1] = 0;
   s->dtxEncSt->lsp_index[2] = 0;

   for ( i = 0; i < DTX_HIST_SIZE; i++ ) {
      memcpy( &s->dtxEncSt->lsp_hist[i * M], lsp_init_data, sizeof( Float32 )*M
            );
   }
   memset( s->dtxEncSt->log_en_hist, 0, M * sizeof( Float32 ) );
   s->dtxEncSt->dtxHangoverCount = DTX_HANG_CONST;
   s->dtxEncSt->decAnaElapsedCount = DTX_ELAPSED_FRAMES_THRESH;

   /* init speech pointers */
   /* New speech */
   s->new_speech = s->old_speech + L_TOTAL - L_FRAME;

   /* Present frame */
   s->speech = s->new_speech - L_NEXT;
   s->p_window = s->old_speech + L_TOTAL - L_WINDOW;

   /* For LPC window				*/
   s->p_window_12k2 = s->p_window - L_NEXT;

   /* Initialize static pointers */
   s->wsp = s->old_wsp + PIT_MAX;
   s->exc = s->old_exc + PIT_MAX + L_INTERPOL;
   s->zero = s->ai_zero + MP1;
   s->error = s->mem_err + M;
   s->h1 = &s->hvec[L_SUBFR];

   /* Static vectors to zero */
   memset( s->old_speech, 0, sizeof( Float32 )*L_TOTAL );
   memset( s->old_exc, 0, sizeof( Float32 )*( PIT_MAX + L_INTERPOL ) );
   memset( s->old_wsp, 0, sizeof( Float32 )*PIT_MAX );
   memset( s->mem_syn, 0, sizeof( Float32 )*M );
   memset( s->mem_w, 0, sizeof( Float32 )*M );
   memset( s->mem_w0, 0, sizeof( Float32 )*M );
   memset( s->mem_err, 0, sizeof( Float32 )*M );
   memset( s->ai_zero, 0, sizeof( Float32 )*L_SUBFR );
   memset( s->hvec, 0, sizeof( Float32 )*L_SUBFR );

   for ( i = 0; i < 5; i++ ) {
      s->old_lags[i] = 40;
   }
   s->sharp = 0.0F;
}


/*
 * cod_amr_init
 *
 *
 * Parameters:
 *    state             O: state structure
 *    dtx               I: dtx mode used
 *
 * Function:
 *    Allocates state memory and initializes state memory
 *
 * Returns:
 *    succeed = 0
 */
static Word32 cod_amr_init( cod_amrState **state, Word32 dtx )
{
   cod_amrState * s;

   if ( ( s = ( cod_amrState * ) malloc( sizeof( cod_amrState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init clLtpState */
   if ( ( s->clLtpSt = ( clLtpState * ) malloc( sizeof( clLtpState ) ) ) == NULL
         ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init Pitch_frState */
   if ( ( s->clLtpSt->pitchSt = ( Pitch_frState * ) malloc( sizeof(
         Pitch_frState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init lspState */
   if ( ( s->lspSt = ( lspState * ) malloc( sizeof( lspState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init Q_plsfState */
   if ( ( s->lspSt->qSt = ( Q_plsfState * ) malloc( sizeof( Q_plsfState ) ) ) ==
         NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init gainQuantState */
   if ( ( s->gainQuantSt = ( gainQuantState * ) malloc( sizeof( gainQuantState )
         ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init gc_predState x2 */
   if ( ( s->gainQuantSt->gc_predSt = ( gc_predState * ) malloc( sizeof(
         gc_predState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   if ( ( s->gainQuantSt->gc_predUncSt = ( gc_predState * ) malloc( sizeof(
         gc_predState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init gain_adaptState */
   if ( ( s->gainQuantSt->adaptSt = ( gain_adaptState * ) malloc( sizeof(
         gain_adaptState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init pitchOLWghtState */
   if ( ( s->pitchOLWghtSt = ( pitchOLWghtState * ) malloc( sizeof(
         pitchOLWghtState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init tonStabState */
   if ( ( s->tonStabSt = ( tonStabState * ) malloc( sizeof( tonStabState ) ) )
         == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init lpcState */
   if ( ( s->lpcSt = ( lpcState * ) malloc( sizeof( lpcState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* init LevinsonState */
   if ( ( s->lpcSt->LevinsonSt = ( LevinsonState * ) malloc( sizeof(
         LevinsonState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   if ( ( s->vadSt = ( vadState * ) malloc( sizeof( vadState ) ) ) == NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }

   /* Init dtx_encState */
   if ( ( s->dtxEncSt = ( dtx_encState * ) malloc( sizeof( dtx_encState ) ) ) ==
         NULL ) {
      DebugPrintf("can not malloc state structure\n" );
      return-1;
   }
   cod_amr_reset( s, dtx );
   *state = s;
   return 0;
}


/*
 * cod_amr_exit
 *
 *
 * Parameters:
 *    state             I: state structure
 *
 * Function:
 *    The memory used for state memory is freed
 *
 * Returns:
 *    Void
 */
static void cod_amr_exit( cod_amrState **state )
{
   if ( state == NULL || *state == NULL )
      return;

   /* deallocate memory */
   free( ( *state )->vadSt );
   free( ( *state )->gainQuantSt->gc_predSt );
   free( ( *state )->gainQuantSt->gc_predUncSt );
   free( ( *state )->gainQuantSt->adaptSt );
   free( ( *state )->clLtpSt->pitchSt );
   free( ( *state )->lspSt->qSt );
   free( ( *state )->lpcSt->LevinsonSt );
   free( ( *state )->lpcSt );
   free( ( *state )->lspSt );
   free( ( *state )->clLtpSt );
   free( ( *state )->gainQuantSt );
   free( ( *state )->pitchOLWghtSt );
   free( ( *state )->tonStabSt );
   free( ( *state )->dtxEncSt );
   free( *state );
   *state = NULL;
   return;
}


/*
 * Speech_Encode_Frame_init
 *
 *
 * Parameters:
 *    state             O: state structure
 *    dtx               I: dtx mode used
 *
 * Function:
 *    Allocates state memory and initializes state memory
 *
 * Returns:
 *    succeed = 0
 */
void * Speech_Encode_Frame_init( int dtx )
{
   Speech_Encode_FrameState * s;

   /* allocate memory */
   if ( ( s = ( Speech_Encode_FrameState * ) malloc( sizeof(
         Speech_Encode_FrameState ) ) ) == NULL ) {
      DebugPrintf("Speech_Encode_Frame_init: can not malloc state "
            "structure\n" );
      return NULL;
   }
   s->pre_state = NULL;
   s->cod_amr_state = NULL;
   s->dtx = dtx;

   if ( Pre_Process_init( &s->pre_state ) || cod_amr_init( &s->cod_amr_state,
         dtx ) ) {
      Speech_Encode_Frame_exit( ( void ** )( &s ) );
      return NULL;
   }
   return s;
}


/*
 * Speech_Encode_Frame_reset
 *
 *
 * Parameters:
 *    state          O: state structure
 *
 * Function:
 *    Resets state memory
 *
 * Returns:
 *
 */
int Speech_Encode_Frame_reset( void *st, int dtx )
{
   Speech_Encode_FrameState * state;
   state = ( Speech_Encode_FrameState * )st;

   if ( ( Speech_Encode_FrameState * )state == NULL ) {
      DebugPrintf("Speech_Encode_Frame_reset: invalid parameter\n" );
      return-1;
   }
   Pre_Process_reset( state->pre_state );
   cod_amr_reset( state->cod_amr_state, dtx );
   return 0;
}


/*
 * Speech_Encode_Frame_exit
 *
 *
 * Parameters:
 *    state            I: state structure
 *
 * Function:
 *    The memory used for state memory is freed
 *
 * Returns:
 *    Void
 */
void Speech_Encode_Frame_exit( void **st )
{
   if ( ( Speech_Encode_FrameState * )( *st ) == NULL )
      return;
   Pre_Process_exit( &( ( ( Speech_Encode_FrameState * )( *st ) )->pre_state ) )
   ;
   cod_amr_exit( &( ( ( Speech_Encode_FrameState * )( *st ) )->cod_amr_state ) )
   ;

   /* deallocate memory */
   free( *st );
   *st = NULL;
   return;
}


/*
 * Speech_Encode_Frame
 *
 *
 * Parameters:
 *    st                B: state structure
 *    mode              I: speech coder mode
 *    new_speech        I: speech input, size L_FRAME
 *    prm               O: Analysis parameters
 *    used_mode         B: force VAD/used_mode
 * Function:
 *    Encode one frame
 *
 * Returns:
 *    Void
 */
void Speech_Encode_Frame( void *st, enum Mode mode, Word16 *new_speech, Word16 *
      prm, enum Mode *used_mode )
{
   Float32 syn[L_FRAME];   /* Buffer for synthesis speech */
   Float32 speech[160];
   Word32 i;


   Speech_Encode_FrameState * state;
   state = ( Speech_Encode_FrameState * )st;

   for ( i = 0; i < 160; i++ ) {
      new_speech[i] = ( Word16 )( new_speech[i] & 0xfff8 );
   }

   /* filter + downscaling */
   Pre_Process( &state->pre_state->y2, &state->pre_state->y1, &state->pre_state
         ->x0, &state->pre_state->x1, new_speech, speech );

   /* Call the speech encoder */
   cod_amr( state->cod_amr_state, mode, speech, prm, used_mode, syn );

}
