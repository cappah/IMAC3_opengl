#include "PerlinNoise.h"

Perlin2D::Perlin2D(int l, int h, int p, int n, float persistence)
{
	m_persistence = persistence;
	m_octaveCount = n;
	m_values.clear();
	m_height = l;
	m_frequency = h;
	m_samplingOffset = p;

	m_maxHeight = (int)ceil(m_height * pow(2, m_octaveCount - 1) / m_samplingOffset);

	m_values.resize(m_maxHeight * m_maxHeight);
	for (int i = 0; i < m_maxHeight * m_maxHeight; i++)
		m_values[i] = ((float)std::rand()) / RAND_MAX;
}

float Perlin2D::getValue2D(int i, int j)
{
	return m_values[i * m_maxHeight + j];
}

float Perlin2D::getNoise2D(float x, float y)
{
	int i = (int)(x / m_samplingOffset);
	int j = (int)(y / m_samplingOffset);
	return interpolation_cos2D(getValue2D(i, j), getValue2D(i + 1, j), getValue2D(i, j + 1), getValue2D(i + 1, j + 1), fmod(x / m_samplingOffset, 1), fmod(y / m_samplingOffset, 1));
}

float Perlin2D::getNoiseValue(float x, float y)
{
	float sum = 0;
	float p = 1;
	int f = 1;
	int i;

	for (i = 0; i < m_octaveCount; i++) {
		sum += p * getNoise2D(x * f, y * f);
		p *= m_persistence;
		f *= 2;
	}
	return sum * (1 - m_persistence) / (1 - p);
}
