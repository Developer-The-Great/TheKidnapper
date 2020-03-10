// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSCameraShakeCPP.h"

UTPSCameraShakeCPP::UTPSCameraShakeCPP()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	RotOscillation.Roll.Frequency = 30.0f;
}
