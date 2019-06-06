#include "Perlin.h"
#include <numeric>
#include <random>

void Perlin::CreateNoise(int seed, int resize)
{
	//fills the vector with resize
	Perm.resize(resize);

	//Fills the permutation vector with value 0.
	iota(Perm.begin(), Perm.end(), 0);

	//creates pseudo random numbers
	default_random_engine DRE(seed);

	//Fills the permutation vector with the new random numbers
	shuffle(Perm.begin(), Perm.end(), DRE);
	Perm.insert(Perm.end(), Perm.begin(), Perm.end());

}

double Perlin::Noise(double x, double y, double z)
{
	//takes the decimal and rounds it down to the next whole number
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = Fade(x);
	double v = Fade(y);
	double w = Fade(z);

	int A = Perm[X] + Y;
	int AA = Perm[A] + Z;
	int AB = Perm[A + 1] + Z;
	int B = Perm[X + 1] + Y;
	int BA = Perm[B] + Z;
	int BB = Perm[B + 1] + Z;

	double BlendedResults = Lerp(w, Lerp(v, Lerp(u, Gradient(Perm[AA], x, y, z), Gradient(Perm[BA], x - 1, y, z)), Lerp(u, Gradient(Perm[AB], x, y - 1, z), Gradient(Perm[BB], x - 1, y - 1, z))), Lerp(v, Lerp(u, Gradient(Perm[AA + 1], x, y, z - 1), Gradient(Perm[BA + 1], x - 1, y, z - 1)), Lerp(u, Gradient(Perm[AB + 1], x, y - 1, z - 1), Gradient(Perm[BB + 1], x - 1, y - 1, z - 1))));
	return (BlendedResults + 1.0) / 2.0;
}

double Perlin::Fade(double xyz)
{
	return xyz * xyz * xyz * (xyz * (xyz * 6 - 15) + 10);
}

double Perlin::Lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double Perlin::Gradient(int hash, double x, double y, double z)
{
	int h = hash & 15;
	double u = h < 8 ? x : y, 
		   v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
