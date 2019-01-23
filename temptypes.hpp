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
	bool operator==(const TempHumid& other)
	{
		return temp == other.temp && humid == other.humid;
	}
	bool operator!=(const TempHumid& other)
	{
		return !(*this==other);
	}
};
