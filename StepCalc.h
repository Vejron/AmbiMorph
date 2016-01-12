#pragma once

// Utility functions to calculate time to make make moves.
// calculations are derived from standard formulas for linear motion and uniform acceleration
namespace StepCalc 
{
	// Calculates maximum speed for accelstepper lib to achive smooth transition over a specified time t.
	double calculateSpeedLimit(double t, uint32_t d, double v_lim, double a_lim)
	{
		double v = 0;

		if ((v_lim * v_lim / a_lim) < d && 2 * v_lim / a_lim < t) 
		{
			// truncated pyramid. Should write a thesis on this one :)
			// v = 1/2 (sqrt(a) sqrt(a t^2-4 d)+a t) and a!=0
			double exp = sqrt(a_lim * t * t - 4 * d);
			v = 0.5 * (a_lim * t - sqrt(a_lim) * exp);
		}
		else 
		{
			// triangle shaped
			v = sqrt(d * a_lim);
		}

		if (v < 0 || v > v_lim)
			v = 0;
		return v;
	}

	// Calculates fastest possible time to move steps given maxspped and acceleration
	double timeToMakeMove(uint32_t steps, double maxSpeed, double acceleration)
	{
		double t = 0;
		if ((maxSpeed * maxSpeed / acceleration) < steps) 
		{
			// truncated pyramid   
			// first the sloping ends   
			t = 2 * maxSpeed / acceleration; // for accel and decel phases   
			steps -= (maxSpeed * maxSpeed / acceleration);
			// then the rectangular centre   
			t += (steps / maxSpeed);
			DEBUG_PRINTLN("truncated triangle");
		}
		else 
		{
			// triangle   
			t = 2 * sqrt(steps / acceleration);
			DEBUG_PRINTLN("sharp triangle");
		}
		return t;
	}
}