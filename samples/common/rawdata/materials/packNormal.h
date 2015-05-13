#pragma once

// Packing normals; math from:
// Martin Mittring - "A bit more Deferred" - CryEngine 3
// http://www.crytek.com/cryengine/cryengine3/presentations/a-bit-more-deferred---cryengine3
// I added an ugly if() because this packing doesn't support (0,0,1) or (0,0,-1)
float2 packNormal(float3 n) {
	if (n.x == 0.0 && n.y == 0.0) {
		n.x += 0.001;
		n = normalize(n);
	}
	return normalize(n.xy) * sqrt(n.z * 0.5 + 0.5);
}

float3 unpackNormal(float2 g) {
	float nz = dot(g.xy, g.xy) * 2 - 1;
	return float3(normalize(g.xy) * sqrt(1 - nz * nz), nz);
}