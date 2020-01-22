
double calcEn(double tof, double dist=1.0){
	if (tof < 0 || dist < 0) return -9999;
	double v = dist / (tof *1.0E-9) / 2.998E8;
	double En = 0.5 * 939.5 * pow(v,2);
	return En;
}
