#ifndef PERLIN
#define PERLIN

#include <iostream>
#include <vector>

using namespace std;

class Perlin
{
	//This vector holds the permutation
	vector<int>Perm;

public:
	void CreateNoise(int seed, int size);

	double Noise(double x, double y, double z);

private:
	double Fade(double t);
	double Lerp(double t, double a, double b);
	double Gradient(int hash, double x, double y, double z);
};

#endif