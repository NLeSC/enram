/******************************************************************************/
/*This program converts KNMI HDF5 radar volume data files to ODIM HDF5 radar  */
/*volume data files.                                                          */
/******************************************************************************/

/*Program: KNMI_vol_h5_to_ODIM_h5.c*/
/*Author: Hidde Leijnse (KNMI)*/
/*Date: January 2012*/  
/*Version: January 2012*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "hdf5.h"
#include "hdf5_hl.h"

/******************************************************************************/
/*Definition of standard parameters.                                          */
/******************************************************************************/
#define STRLEN       (128)               /*Length of all strings used.*/
#define NDSETX       (128)

struct scanmeta {
	int date;             /*Date of scan data in YYYYMMDD.*/
	int time;             /*Time of scan data in HHMMSS.*/
	float elev;           /*Elevation of scan in deg.*/
	int nrang;            /*Number of range bins in scan.*/
	int nazim;            /*Number of azimuth rays in scan.*/
	float rscale;         /*Size of range bins in scan in km.*/
	float ascale;         /*Size of azimuth steps in scan in deg.*/
	int azim0;            /*Ray number with which radar scan started.*/
	int missing;          /*Missing value of quantity contained by scan.*/
	float PRFh;           /*High PRF used for scan in Hz.*/
	float PRFl;           /*Low PRF used for scan in Hz.*/
	float pulse;          /*Pulse length in microsec.*/
	float radcnst;        /*Radar constant in dB.*/
	float txnom;          /*Nominal maximum TX power in kW.*/
	float antvel;         /*Antenna velocity in deg/s.*/	
	float lon;            /*Longitude of radar in deg.*/	
	float lat;            /*Latitude of radar in deg.*/
	float Z_offset;       /*Offset value of quantity contained by scan.*/
	float Z_scale;        /*Scale of value of quantity contained by scan.*/
	float uZ_offset;      /*Offset value of quantity contained by scan.*/
	float uZ_scale;       /*Scale of value of quantity contained by scan.*/
	float V_offset;       /*Offset value of quantity contained by scan.*/
	float V_scale;        /*Scale of value of quantity contained by scan.*/
	float W_offset;       /*Offset value of quantity contained by scan.*/
	float W_scale;        /*Scale of value of quantity contained by scan.*/
};
typedef struct scanmeta SCANMETA;

int read_h5_scan(hid_t file, int iscan, SCANMETA *meta);
void string2datetime(char *string, int *date, int *time);

int main(int argc, char **argv)
{
	hid_t h5_in, h5_out;
	char object_in[STRLEN], object[STRLEN], string[STRLEN], id_string_DeBilt[STRLEN], id_string_DenHelder[STRLEN];
	double x_double, lat, lon, lon_DenHelder, height_DeBilt, height_DenHelder;
	int correct_calib_error, correct_lon_DH, i, t, Nscan, radnum;
	float radloc[2];
	long x_long;
	SCANMETA *meta;
	
	/*Set values.*/
	correct_calib_error = 1;
	correct_lon_DH = 1;
	height_DeBilt = 44;
	height_DenHelder = 51;
	sprintf(id_string_DeBilt, "RAD:NL50,NOD:nldbl,PLC:De Bilt");
	sprintf(id_string_DenHelder, "RAD:NL51,NOD:nldhl,PLC:Den Helder");
	lon_DenHelder = 4.789997;
	
	/*Check number of input arguments.*/
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s ODIM_file.h5 KNMI_input_file.h5\n", argv[0]);
		exit(1);
	}
	
	/*Open input file.*/
	if ((h5_in = H5Fopen(argv[2], H5F_ACC_RDONLY, H5P_DEFAULT)) < 0)
	{
		fprintf(stderr, "Error: KNMI HDF5 input file %s could not be opened!\n", argv[2]);
		exit(2);
	}
	
	/*Open output file.*/
	if ((h5_out = H5Fcreate(argv[1], H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)) < 0)
	{
		fprintf(stderr, "Error: ODIM HDF5 output file %s could not be opened!\n", argv[1]);
		H5Fclose(h5_in);
		exit(3);
	}
	
	/*Read KNMI volume data file.*/
	H5LTget_attribute_int(h5_in, "/overview", "number_scan_groups", &Nscan);
	meta = (SCANMETA *) malloc(Nscan * sizeof(SCANMETA));
	for (i = 0; i < Nscan; i++)
	{
		if (read_h5_scan(h5_in, i + 1, &(meta[i])) == 0)
		{
			Nscan = i;
			break;
		}
	}
	
	/*Read radar information.*/
	H5LTget_attribute_string(h5_in, "/radar1", "radar_name", string);
	if (strcmp(string, "De_Bilt") == 0) radnum = 60;
	if (strcmp(string, "Den_Helder") == 0) radnum = 61;
	H5LTget_attribute_float(h5_in, "/radar1", "radar_location", radloc);
	lon = (double) radloc[0];
	lat = (double) radloc[1];
	if ((radnum == 61) && (correct_lon_DH == 1)) lon = lon_DenHelder;	
	
	/*Create root attribute.*/
	H5LTset_attribute_string(h5_out, "/", "Conventions", "ODIM_H5/V2_1");
	
	/*Create and populate top-level what-group.*/
	H5Gcreate(h5_out, "/what", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	H5LTset_attribute_string(h5_out, "/what", "object", "PVOL");
	H5LTset_attribute_string(h5_out, "/what", "version", "H5rad 2.1");
	sprintf(string, "%08d", meta[0].date);
	H5LTset_attribute_string(h5_out, "/what", "date", string);
	sprintf(string, "%06d", meta[0].time);
	H5LTset_attribute_string(h5_out, "/what", "time", string);
	if (radnum == 60) H5LTset_attribute_string(h5_out, "/what", "source", id_string_DeBilt);
	if (radnum == 61) H5LTset_attribute_string(h5_out, "/what", "source", id_string_DenHelder);
	
	
	/*Create and populate top-level where-group.*/
	H5Gcreate(h5_out, "/where", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	H5LTset_attribute_double(h5_out, "/where", "lon", &lon, 1);
	H5LTset_attribute_double(h5_out, "/where", "lat", &lat, 1);
	if (radnum == 60) x_double = height_DeBilt;
	if (radnum == 61) x_double = height_DenHelder;
	H5LTset_attribute_double(h5_out, "/where", "height", &x_double, 1);
	
	/*Create and populate different scan groups.*/
	for (i = 0; i < Nscan; i++)
	{
		sprintf(object, "/dataset%d", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Create and populate what-group.*/
		sprintf(object, "/dataset%d/what", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5LTset_attribute_string(h5_out, object, "product", "SCAN");
		sprintf(string, "%08d", meta[i].date);
		H5LTset_attribute_string(h5_out, object, "startdate", string);
		sprintf(string, "%06d", meta[i].time);
		H5LTset_attribute_string(h5_out, object, "starttime", string);
		sprintf(string, "%08d", meta[i].date);
		H5LTset_attribute_string(h5_out, object, "enddate", string);
		t = meta[i].time % 100 + ((int) (360.0 / meta[i].antvel));
		if (t >= 60) t += 40;
		t += (meta[i].time / 100) * 100;
		sprintf(string, "%06d", t);
		H5LTset_attribute_string(h5_out, object, "endtime", string);
		
		/*Create and populate where-group.*/
		sprintf(object, "/dataset%d/where", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		x_double = (double) meta[i].elev;
		H5LTset_attribute_double(h5_out, object, "elangle", &x_double, 1);
		x_long = (long) meta[i].elev;
		H5LTset_attribute_long(h5_out, object, "nrang", &x_long, 1);
		x_double = 0.0;
		H5LTset_attribute_double(h5_out, object, "rstart", &x_double, 1);
		x_double = (double) meta[i].rscale;
		H5LTset_attribute_double(h5_out, object, "rscale", &x_double, 1);
		x_long = (long) meta[i].nazim;
		H5LTset_attribute_long(h5_out, object, "nrays", &x_long, 1);
		x_long = (long) meta[i].azim0;
		H5LTset_attribute_long(h5_out, object, "a1gate", &x_long, 1);
		
		/*Create and populate groups for the data.*/
		sprintf(object, "/dataset%d/data1", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Copy data from KNMI to ODIM file.*/
		sprintf(object, "/dataset%d/data1/data", i + 1);
		sprintf(object_in, "/scan%d/scan_Z_data", i + 1);
		H5Ocopy(h5_in, object_in, h5_out, object, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Create and populate data-what group.*/
		sprintf(object, "/dataset%d/data1/what", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5LTset_attribute_string(h5_out, object, "quantity", "DBZH");
		x_double = (double) meta[i].Z_scale;
		H5LTset_attribute_double(h5_out, object, "gain", &x_double, 1);
		x_double = (double) meta[i].Z_offset;
		if (correct_calib_error == 1) x_double -= (double) meta[i].Z_scale;
		H5LTset_attribute_double(h5_out, object, "offset", &x_double, 1);
		x_double = 255.0;
		H5LTset_attribute_double(h5_out, object, "nodata", &x_double, 1);
		x_double = (double) meta[i].missing;
		H5LTset_attribute_double(h5_out, object, "undetect", &x_double, 1);
		
		sprintf(object, "/dataset%d/data2", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Copy data from KNMI to ODIM file.*/
		sprintf(object, "/dataset%d/data2/data", i + 1);
		sprintf(object_in, "/scan%d/scan_uZ_data", i + 1);
		H5Ocopy(h5_in, object_in, h5_out, object, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Create and populate data-what group.*/
		sprintf(object, "/dataset%d/data2/what", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5LTset_attribute_string(h5_out, object, "quantity", "TH");
		x_double = (double) meta[i].uZ_scale;
		H5LTset_attribute_double(h5_out, object, "gain", &x_double, 1);
		x_double = (double) meta[i].uZ_offset;
		if (correct_calib_error == 1) x_double -= (double) meta[i].uZ_scale;
		H5LTset_attribute_double(h5_out, object, "offset", &x_double, 1);
		x_double = 255.0;
		H5LTset_attribute_double(h5_out, object, "nodata", &x_double, 1);
		x_double = (double) meta[i].missing;
		H5LTset_attribute_double(h5_out, object, "undetect", &x_double, 1);
		
		sprintf(object, "/dataset%d/data3", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Copy data from KNMI to ODIM file.*/
		sprintf(object, "/dataset%d/data3/data", i + 1);
		sprintf(object_in, "/scan%d/scan_V_data", i + 1);
		H5Ocopy(h5_in, object_in, h5_out, object, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Create and populate data-what group.*/
		sprintf(object, "/dataset%d/data3/what", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5LTset_attribute_string(h5_out, object, "quantity", "VRAD");
		x_double = (double) meta[i].V_scale;
		H5LTset_attribute_double(h5_out, object, "gain", &x_double, 1);
		x_double = (double) meta[i].V_offset;
		if (correct_calib_error == 1) x_double -= (double) meta[i].V_scale;
		H5LTset_attribute_double(h5_out, object, "offset", &x_double, 1);
		x_double = 255.0;
		H5LTset_attribute_double(h5_out, object, "nodata", &x_double, 1);
		x_double = (double) meta[i].missing;
		H5LTset_attribute_double(h5_out, object, "undetect", &x_double, 1);
		
		sprintf(object, "/dataset%d/data4", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Copy data from KNMI to ODIM file.*/
		sprintf(object, "/dataset%d/data4/data", i + 1);
		sprintf(object_in, "/scan%d/scan_W_data", i + 1);
		H5Ocopy(h5_in, object_in, h5_out, object, H5P_DEFAULT, H5P_DEFAULT);
		
		/*Create and populate data-what group.*/
		sprintf(object, "/dataset%d/data4/what", i + 1);
		H5Gcreate(h5_out, object, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		H5LTset_attribute_string(h5_out, object, "quantity", "WRAD");
		x_double = (double) meta[i].W_scale;
		H5LTset_attribute_double(h5_out, object, "gain", &x_double, 1);
		x_double = (double) meta[i].W_offset;
		if (correct_calib_error == 1) x_double -= (double) meta[i].W_scale;
		H5LTset_attribute_double(h5_out, object, "offset", &x_double, 1);
		x_double = 255.0;
		H5LTset_attribute_double(h5_out, object, "nodata", &x_double, 1);
		x_double = (double) meta[i].missing;
		H5LTset_attribute_double(h5_out, object, "undetect", &x_double, 1);
	}
	
	/*Close files.*/
	H5Fclose(h5_in);
	H5Fclose(h5_out);
	
	/*End of program.*/
	return 0;
}



int read_h5_scan(hid_t file, int iscan, SCANMETA *meta)
{
	char object[STRLEN], string[STRLEN];
	int Nscan = 0;
	
	/*Checking requested scan number.*/
	H5LTget_attribute_int(file, "/overview","number_scan_groups", &Nscan);
	if ((iscan < 1) || (iscan > Nscan))
	{
		fprintf(stderr, "Requested scan is not available in this file\n");
		return 0;
	}
	
	/*Reading of metadata from scan group in file.*/
	sprintf(object, "/scan%d", iscan);
	H5LTget_attribute_string(file, object, "scan_datetime", string);
	string2datetime(string, &((*meta).date), &((*meta).time));
	H5LTget_attribute_float(file, object, "scan_elevation", &((*meta).elev));
	H5LTget_attribute_int(file, object, "scan_number_range", &((*meta).nrang));
	H5LTget_attribute_int(file, object, "scan_number_azim", &((*meta).nazim));
	H5LTget_attribute_float(file, object, "scan_range_bin", &((*meta).rscale));
	H5LTget_attribute_float(file, object, "scan_azim_bin", &((*meta).ascale));
	H5LTget_attribute_int(file, object, "scan_start_azim", &((*meta).azim0));
	H5LTget_attribute_float(file, object, "scan_low_PRF", &((*meta).PRFl));
	H5LTget_attribute_float(file, object, "scan_high_PRF", &((*meta).PRFh));
	H5LTget_attribute_float(file, object, "scan_pulse_width", &((*meta).pulse));
	H5LTget_attribute_float(file, object, "scan_radar_constant", &((*meta).radcnst));
	H5LTget_attribute_float(file, object, "scan_TX_power_nom", &((*meta).txnom));
	H5LTget_attribute_float(file, object, "scan_antenna_velocity", &((*meta).antvel));
	
	/*Reading of metadata from scan/calibration subgroup in file.*/
	sprintf(object, "/scan%d/calibration", iscan);
	H5LTget_attribute_string(file, object, "calibration_Z_formulas", string);
	sscanf(string,"GEO=%f*PV+%f", &((*meta).Z_scale), &((*meta).Z_offset));
	H5LTget_attribute_string(file, object, "calibration_uZ_formulas", string);
	sscanf(string,"GEO=%f*PV+%f", &((*meta).uZ_scale), &((*meta).uZ_offset));
	H5LTget_attribute_string(file, object, "calibration_V_formulas", string);
	sscanf(string,"GEO=%f*PV+%f", &((*meta).V_scale), &((*meta).V_offset));
	H5LTget_attribute_string(file, object, "calibration_W_formulas", string);
	sscanf(string,"GEO=%f*PV+%f", &((*meta).W_scale), &((*meta).W_offset));
	H5LTget_attribute_int(file, object, "calibration_missing_data", &((*meta).missing));
	
	/*Returning.*/
	return 1;
}


/******************************************************************************/
/*This function converts a string according to the KNMI HDF5 specification to */
/*a date (yyyymmdd) and time (hhmmss).                                        */
/******************************************************************************/
void string2datetime(char *string, int *date, int *time)
{
	char months[13][4]={"XXX", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
	char month[4];
	int yy=0, mon=0, dd=0, hh=0, mm=0, ss=0, m;
	
	/*Extraction of year, month, day, etc from string.*/
	sscanf(string, "%d-%[^-]-%d;%d:%d:%d", &dd, month, &yy, &hh, &mm, &ss);
	for (m = 1; m <= 12; m++)
	{
		if (strncmp(month, months[m], 3) == 0) mon = m;
	}
	
	/*Returning date and time in integer format.*/
	(*date) = 10000 * yy + 100 * mon + dd;
	(*time) = 10000 * hh + 100 * mm + ss;
}

