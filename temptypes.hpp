#pragma once

struct TempHumid
{
	float temp;
	float humid;
	TempHumid(float temperature, float humidity) : temp(temperature), humid(humidity){};
	TempHumid()
	{
		TempHumid(0,0);
	}
};